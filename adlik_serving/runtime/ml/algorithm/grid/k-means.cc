// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/ml_task.h"
#include "adlik_serving/runtime/ml/algorithm/proto/task_config.pb.h"
#include "csv/reader.hpp"
#include "csv/writer.hpp"
#include "cub/env/fs/file_system.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "cub/string/string_view.h"
#include "dlib/clustering.h"
#include "dlib/rand.h"

namespace ml_runtime {

namespace {

using SampleType = dlib::matrix<double, 0, 1>;
using KernelType = dlib::radial_basis_kernel<SampleType>;

std::vector<SampleType> loadCSV(const std::string& file_path) {
  // using Row = csv::unordered_flat_map<std::string_view, std::string>;
  using Row = csv::unordered_flat_map<cub::StringView, std::string>;

  std::vector<SampleType> samples;

  auto func = [&](Row& row) {
    SampleType s;
    size_t i = 0;
    s.set_size(row.size());
    for (auto& col : row) {
      auto d = std::stod(col.second);
      s(i) = d;
      i++;
    }
    samples.push_back(s);
  };

  try {
    csv::Reader reader;
    reader.read(file_path);
    while (reader.busy()) {
      if (reader.ready()) {
        auto row = reader.next_row();
        func(row);
      }
    }
  } catch (...) {
    ERR_LOG << "catch (...)";
  }

  return samples;
}

}  // namespace

struct KMeans : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>*);

  cub::StatusWrapper run(const adlik::serving::TaskReq&, adlik::serving::TaskRsp&) override;
  const std::string name() const override {
    return "k-means";
  }
};

void KMeans::create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>* algorithm) {
  *algorithm = std::make_unique<KMeans>();
  // TODO: should have some other parameters to set, expecially for prediction scene
}

// should try catch exceptions
cub::StatusWrapper KMeans::run(const adlik::serving::TaskReq& req, adlik::serving::TaskRsp& rsp) {
  DEBUG_LOG << "Prepare to run k-means";

  const auto& config = req.grid();
  if (!cub::filesystem().exists(config.input())) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input file doesn't exist");
  }

  if (config.n_clusters() <= 0) {
    return cub::StatusWrapper(cub::InvalidArgument, "k <= 0!");
  }

  if (config.compute_labels()) {
    auto path = cub::Path(config.output());
    auto dir = path.dirName();
    if (!cub::filesystem().exists(dir.to_s())) {
      return cub::StatusWrapper(cub::InvalidArgument, "Output directory doesn't exist");
    }
  }

  DEBUG_LOG << "Prepare to load samples";
  std::vector<SampleType> samples = loadCSV(config.input());
  DEBUG_LOG << "Samples size: " << samples.size() << ", k=" << config.n_clusters();

  dlib::kcentroid<KernelType> kc(KernelType(0.1), 0.01);  // Should config?
  dlib::kkmeans<KernelType> method(kc);
  std::vector<SampleType> initial_centers;

  method.set_number_of_centers(config.n_clusters());
  dlib::pick_initial_centers(config.n_clusters(), initial_centers, samples, method.get_kernel());
  method.train(samples, initial_centers, config.max_iter());

  if (config.compute_labels()) {
    csv::Writer writer(config.output());
    if (config.label_name().length() > 0) {
      writer.configure_dialect().column_names(config.label_name());
    }
    for (unsigned long i = 0; i < samples.size(); ++i) {
      writer.write_row(std::to_string(method(samples[i])));
    }
    writer.close();
  }

  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(KMeans, "k-means");

}  // namespace ml_runtime
