// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <vector>

#include "dlib/clustering.h"
#include "dlib/rand.h"

namespace ml_runtime {

template <typename SampleType>
struct Kmeans {
#if 0
  std::vector<unsigned long> operator()(unsigned long n_clusters,
                                        std::vector<SampleType>& samples,
                                        long max_iter = 1000) {
    using KernelType = dlib::radial_basis_kernel<SampleType>;

    dlib::kcentroid<KernelType> kc(KernelType(0.1), 0.01);  // Should config?
    dlib::kkmeans<KernelType> km(kc);
    std::vector<SampleType> initial_centers;

    km.set_number_of_centers(n_clusters);
    dlib::pick_initial_centers(n_clusters, initial_centers, samples, km.get_kernel());
    km.train(samples, initial_centers, max_iter);

    std::vector<unsigned long> results;
    for (const auto& it : samples) {
      results.push_back(km(it));
    }

    // should return centers in addition
    return results;
  }
#endif

  std::vector<unsigned long> operator()(unsigned long n_clusters,
                                        std::vector<SampleType>& samples,
                                        long max_iter = 1000) {
    std::vector<SampleType> centers;
    dlib::pick_initial_centers(n_clusters, centers, samples, dlib::linear_kernel<SampleType>());

    dlib::find_clusters_using_kmeans(samples, centers, max_iter);
    if (centers.size() != n_clusters) {
      // should exception process
    }

    std::vector<unsigned long> results;
    std::vector<int> hits(centers.size(), 0);
    for (unsigned long i = 0; i < samples.size(); ++i) {
      unsigned long best_idx = 0;
      double best_dist = 1e100;
      for (unsigned long j = 0; j < centers.size(); ++j) {
        if (length(samples[i] - centers[j]) < best_dist) {
          best_dist = length(samples[i] - centers[j]);
          best_idx = j;
        }
      }
      results[i] = best_idx;
    }

    return results;
  }
};

}  // namespace ml_runtime
