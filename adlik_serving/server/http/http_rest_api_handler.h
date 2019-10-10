// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_HTTP_HTTP_REST_API_HANDLER_H
#define ADLIK_SERVING_SERVER_HTTP_HTTP_REST_API_HANDLER_H

#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "adlik_serving/server/http/internal/request_handler_options.h"
#include "re2/re2.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/default/integral_types.h"

namespace adlik {
namespace serving {

struct GetModelMetaImpl;
struct PredictImpl;

struct HttpRestApiHandler {
  HttpRestApiHandler(GetModelMetaImpl&, PredictImpl&, const RequestHandlerOptions&);

  tensorflow::Status processRequest(const absl::string_view http_method,
                                    const absl::string_view request_path,
                                    const std::string& request_body,
                                    std::vector<std::pair<std::string, std::string>>* headers,
                                    std::string* output);

  static const char* const kPathRegex;

private:
  tensorflow::Status processPredictRequest(const absl::string_view model_name,
                                           const absl::optional<tensorflow::int64>& model_version,
                                           const std::string& request_body,
                                           std::string* output);

  tensorflow::Status processModelMetadataRequest(const absl::string_view model_name,
                                                 const absl::string_view model_version_str,
                                                 std::string* output);

  GetModelMetaImpl& get_model_meta_impl;
  PredictImpl& predict_impl;
  RequestHandlerOptions options;
  const RE2 prediction_api_regex;
  const RE2 modelstatus_api_regex;
};

}  // namespace serving
}  // namespace adlik

#endif
