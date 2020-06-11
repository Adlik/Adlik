// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <cmath>
#include <iostream>
#include <numeric>
#include "adlik_serving/apis/predict.pb.h"
#include "adlik_serving/demo/decode_png.h"
#include "adlik_serving/demo/predict_service.h"

namespace chrono = std::chrono;
namespace demo = adlik::serving::demo;

using adlik::serving::ModelInput;
using adlik::serving::PredictRequest;
using adlik::serving::PredictRequest_Output;
using adlik::serving::PredictResponse;
using adlik::serving::demo::PredictService;
using google::protobuf::MapPair;
using google::protobuf::RepeatedField;
using std::string;
using std::tuple;
using std::vector;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using tensorflow::DataType;
using tensorflow::TensorProto;

namespace {
template <class T>
struct Statistics {
  T min;
  T max;
  T average;
  T standard_deviation;
  T total;
};

TensorProto make_tensor_proto(const RepeatedField<int64_t>& dims, const char* image_path) {
  if (dims.size() != 3) {
    throw std::runtime_error{"Invalid model input dimensions."};
  }

  const auto image = demo::decode_png(image_path, static_cast<size_t>(dims[2]));

  if (image.get_height() != static_cast<size_t>(dims[0]) || image.get_width() != static_cast<size_t>(dims[1]) ||
      image.get_channels() != static_cast<size_t>(dims[2])) {
    throw std::runtime_error{"Invalid image dimensions."};
  }

  TensorProto result;

  result.set_dtype(DataType::DT_FLOAT);

  result.mutable_tensor_shape()->add_dim()->set_size(1);
  result.mutable_tensor_shape()->add_dim()->set_size(image.get_height());
  result.mutable_tensor_shape()->add_dim()->set_size(image.get_width());
  result.mutable_tensor_shape()->add_dim()->set_size(image.get_channels());

  const auto num_pixels = image.get_width() * image.get_height() * image.get_channels();

  for (auto it = image.get_data(), last = image.get_data() + num_pixels; it != last; ++it) {
    union {
      float source;
      char target[sizeof(float)];
    } converter;

    converter.source = *it;

    std::copy(
        std::begin(converter.target), std::end(converter.target), std::back_inserter(*result.mutable_tensor_content()));
  }

  return result;
}

PredictRequest make_predict_request(const char* model_name,
                                    const ModelInput& model_input,
                                    const char* image_path,
                                    const string& output_name) {
  PredictRequest request;

  request.mutable_model_spec()->set_name(model_name);
  request.set_batch_size(1);

  request.mutable_inputs()->insert(
      MapPair<string, TensorProto>{model_input.name(), make_tensor_proto(model_input.dims(), image_path)});

  request.mutable_output_filter()->insert(MapPair<string, PredictRequest_Output>{output_name, PredictRequest_Output()});

  return request;
}

Statistics<steady_clock::duration> get_statistics(const vector<steady_clock::rep>& data) {
  const auto min_max = std::minmax_element(data.begin(), data.end());
  const auto sum = std::accumulate(data.begin(), data.end(), steady_clock::rep{});
  const auto average = static_cast<double>(sum) / static_cast<double>(data.size());
  auto variance_sum = 0.0;

  for (auto value : data) {
    const auto diff = static_cast<double>(value) - average;

    variance_sum += diff * diff;
  }

  const auto standard_variance =
      static_cast<steady_clock::rep>(std::sqrt(variance_sum / static_cast<double>(data.size())));

  return {steady_clock::duration{*min_max.first},
          steady_clock::duration{*min_max.second},
          steady_clock::duration{static_cast<steady_clock::rep>(average)},
          steady_clock::duration{standard_variance},
          steady_clock::duration{sum}};
}
}  // namespace

int main(int argc, char* argv[]) {
  // Collect arguments.

  const char* model_repository;
  const char* model_name;
  const char* image_path;
  size_t samples = 32;

  switch (argc) {
    case 4:
      model_repository = argv[1];
      model_name = argv[2];
      image_path = argv[3];

      break;
    case 5:
      model_repository = argv[1];
      model_name = argv[2];
      image_path = argv[3];

      errno = 0;

      samples = static_cast<size_t>(std::strtoul(argv[4], nullptr, 10));

      if (errno != 0) {
        errno = 0;

        std::cerr << "Invalid sample specified.";

        return 1;
      }

      break;
    default:
      std::cerr << "Usage: " << argv[0] << " <MODEL_REPOSITORY> <MODEL_NAME> <IMAGE_PATH> [SAMPLES = " << samples
                << "]\n";

      return 1;
  }

  if (samples == 0) {
    std::cerr << "SAMPLES must be a positive integer.";

    return 1;
  }

  // Start service.

  PredictService service{model_repository};

  service.start();

  // Fetch model configuration.

  const auto model_config = service.getModelConfig(model_name);

  if (model_config == nullptr) {
    std::cerr << "Model not found: " << model_name << ".\n";

    return 1;
  }

  const auto& model_input = model_config->input(0);

  if (model_input.data_type() != DataType::DT_FLOAT) {
    std::cerr << "Only float32 input is supported.\n";

    return 1;
  }

  const auto& model_output = model_config->output(0);

  if (model_output.data_type() != DataType::DT_FLOAT) {
    std::cerr << "Only float32 output is supported.\n";

    return 1;
  }

  if (model_output.dims_size() != 1) {
    std::cerr << "Only categorical output is supported.\n";

    return 1;
  }

  // Prepare request.

  const auto request = make_predict_request(model_name, model_input, image_path, model_output.name());

  // Get response.

  PredictResponse response;

  vector<steady_clock::rep> predict_times(samples, steady_clock::rep{});
  const auto start_time = steady_clock::now().time_since_epoch().count();

  for (size_t i = 0; i != samples; ++i) {
    service.predict(request, response);
    predict_times[i] = steady_clock::now().time_since_epoch().count();
  }

  // Calculate predict times differences.

  for (size_t i = samples - 2; i < samples; --i) {
    predict_times[i + 1] -= predict_times[i];
  }

  predict_times[0] -= start_time;

  const auto statistics = get_statistics(predict_times);

  // Sort result.

  const auto& output = *response.outputs().begin();
  const auto num_results = output.second.tensor().tensor_content().size() / sizeof(float);
  vector<tuple<int, double>> probabilities;

  for (size_t i = 0; i != num_results; ++i) {
    union {
      char source[sizeof(float)];
      float target;
    } converter;

    std::copy_n(output.second.tensor().tensor_content().data() + sizeof(float) * i, sizeof(float), converter.source);

    probabilities.emplace_back(i, converter.target);
  }

  std::sort(probabilities.begin(), probabilities.end(), [](const auto& lhs, const auto& rhs) {
    return std::get<1>(rhs) < std::get<1>(lhs);
  });

  // Print result.

  std::cout << "\n"
            << "           Samples: " << samples << ".\n"
            << "  Minimum duration: " << chrono::duration_cast<milliseconds>(statistics.min).count() << " ms.\n"
            << "  Maximum duration: " << chrono::duration_cast<milliseconds>(statistics.max).count() << " ms.\n"
            << "  Average duration: " << chrono::duration_cast<milliseconds>(statistics.average).count() << " ms.\n"
            << "Standard deviation: " << chrono::duration_cast<milliseconds>(statistics.standard_deviation).count()
            << " ms.\n"
            << "             Speed: " << 1.0 / chrono::duration_cast<duration<double>>(statistics.average).count()
            << " Hz.\n"
            << "    Total duration: " << chrono::duration_cast<milliseconds>(statistics.total).count() << " ms.\n\n"
            << "Probability distribution:\n\n";

  if (probabilities.size() > 10) {
    probabilities.resize(10);
  }

  for (const auto& item : probabilities) {
    std::cout << std::get<0>(item) << " => " << std::get<1>(item) << '\n';
  }
}
