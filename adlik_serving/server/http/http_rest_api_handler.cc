// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/http/http_rest_api_handler.h"

#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "adlik_serving/apis/get_model_meta.pb.h"
#include "adlik_serving/apis/get_model_meta_impl.h"
#include "adlik_serving/apis/predict.pb.h"
#include "adlik_serving/apis/predict_impl.h"
#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/apis/task_op_impl.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "cub/log/log.h"
#include "google/protobuf/util/json_util.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

namespace {
void addHeaders(std::vector<std::pair<std::string, std::string>>* headers) {
  headers->push_back({"Content-Type", "application/json"});
}

void fillJsonErrorMsg(const std::string& errmsg, std::string* output) {
  // Errors are represented as following JSON object:
  // {
  //   "error": "<CEscaped error message string>"
  // }
  absl::StrAppend(output, R"({ "error": ")", absl::CEscape(errmsg), R"(" })");
}

RunOptions runOptions(const RequestHandlerOptions& options) {
  RunOptions opts;
  opts.timeout_in_ms = options.timeout_in_ms();
  return opts;
}

tensorflow::Status serializeMessage(const ::google::protobuf::Message& message, std::string* output) {
  ::google::protobuf::util::JsonPrintOptions opts;
  opts.add_whitespace = true;
  opts.always_print_primitive_fields = false;
  opts.preserve_proto_field_names = true;

  std::string response_output;
  const auto& status = MessageToJsonString(message, output, opts);
  return status.ok() ?
             tensorflow::Status::OK() :
             tensorflow::errors::Internal("Failed to convert protobuf message to json. Error: ", status.ToString());
}

tensorflow::Status unserializeMessage(const std::string& json_str, ::google::protobuf::Message* message) {
  ::google::protobuf::util::JsonParseOptions parseOpt;
  parseOpt.ignore_unknown_fields = true;
  const auto& status = JsonStringToMessage(json_str, message, parseOpt);
  return status.ok() ? tensorflow::Status::OK() :
                       tensorflow::errors::Internal(
                           "Failed to convert json str to protobuf message, input "
                           "json str legnth: ",
                           json_str.size(),
                           " Error: ",
                           status.ToString());
}

}  // namespace

const char* const HttpRestApiHandler::kPathRegex = "(?i)/v1/.*";

HttpRestApiHandler::HttpRestApiHandler(GetModelMetaImpl& meta_impl,
                                       PredictImpl& predict_impl,
                                       TaskOpImpl& task_op_impl,
                                       const RequestHandlerOptions& options)
    : get_model_meta_impl(meta_impl),
      predict_impl(predict_impl),
      task_op_impl(task_op_impl),
      options(options),
      prediction_api_regex(R"((?i)/v1/models/([^/:]+)(?:/versions/(\d+))?:(predict))"),
      prediction_ml_api_regex(R"((?i)/v1/models/([^/:]+)(?:/versions/(\d+))?:(ml_predict))"),
      model_status_api_regex(R"((?i)/v1/models(?:/([^/:]+))?(?:/versions/(\d+))?(?:\/(metadata))?)") {
}

tensorflow::Status HttpRestApiHandler::processRequest(const absl::string_view http_method,
                                                      const absl::string_view request_path,
                                                      const std::string& request_body,
                                                      std::vector<std::pair<std::string, std::string>>* headers,
                                                      std::string* output) {
  INFO_LOG << "Receive request: method: " << http_method << ", path: " << request_path;

  headers->clear();
  output->clear();
  addHeaders(headers);
  std::string model_name;
  std::string model_version_str;
  std::string method;
  std::string model_subresource;
  tensorflow::Status status =
      tensorflow::errors::InvalidArgument("Malformed request: ", http_method, " ", request_path);
  if (http_method == "POST" &&
      RE2::FullMatch(std::string(request_path), prediction_api_regex, &model_name, &model_version_str, &method)) {
    absl::optional<tensorflow::int64> model_version;
    if (!model_version_str.empty()) {
      tensorflow::int64 version;
      if (!absl::SimpleAtoi(model_version_str, &version)) {
        return tensorflow::errors::InvalidArgument("Failed to convert version: ", model_version_str, " to numeric.");
      }
      model_version = version;
    }

    status = processPredictRequest(model_name, model_version, request_body, output);

  } else if (http_method == "POST" &&
             RE2::FullMatch(
                 std::string(request_path), prediction_ml_api_regex, &model_name, &model_version_str, &method)) {
    absl::optional<tensorflow::int64> model_version;
    if (!model_version_str.empty()) {
      tensorflow::int64 version;
      if (!absl::SimpleAtoi(model_version_str, &version)) {
        return tensorflow::errors::InvalidArgument("Failed to convert version: ", model_version_str, " to numeric.");
      }
      model_version = version;
    }
    status = processMlPredictRequest(model_name, model_version, request_body, output);
  } else if (http_method == "GET" && RE2::FullMatch(std::string(request_path),
                                                    model_status_api_regex,
                                                    &model_name,
                                                    &model_version_str,
                                                    &model_subresource)) {
    INFO_LOG << "Receive GET request, model_name: " << model_name << ", version: " << model_version_str
             << ", sub: " << model_subresource;
    if (!model_subresource.empty() && model_subresource == "metadata") {
      status = processModelMetadataRequest(model_name, model_version_str, output);
    } else {
      return tensorflow::errors::InvalidArgument("Not found handler for request method ", model_subresource);
    }
  }

  if (!status.ok()) {
    fillJsonErrorMsg(status.error_message(), output);
  }
  return status;
}

tensorflow::Status HttpRestApiHandler::processPredictRequest(const absl::string_view model_name,
                                                             const absl::optional<tensorflow::int64>& model_version,
                                                             const std::string& request_body,
                                                             std::string* output) {
  if (model_name.empty()) {
    return tensorflow::errors::InvalidArgument("Missing model name in request.");
  }

  PredictRequest request;
  TF_RETURN_IF_ERROR(unserializeMessage(request_body, &request));

  request.mutable_model_spec()->set_name(std::string(model_name));
  if (model_version.has_value()) {
    request.mutable_model_spec()->mutable_version()->set_value(model_version.value());
  }

  //   TF_RETURN_IF_ERROR(
  //       FillPredictRequestFromJson(request_body, *model_config, request, format));

  PredictResponse response;
  TF_RETURN_IF_ERROR(predict_impl.predict(runOptions(options), request, response));
  //   TF_RETURN_IF_ERROR(MakeJsonFromOutputs(response.outputs(), format, output));

  std::string response_output;
  TF_RETURN_IF_ERROR(serializeMessage(response, &response_output));
  absl::StrAppend(output, response_output);
  return tensorflow::Status::OK();
}

tensorflow::Status HttpRestApiHandler::processMlPredictRequest(const absl::string_view model_name,
                                                               const absl::optional<tensorflow::int64>& model_version,
                                                               const std::string& request_body,
                                                               std::string* output) {
  if (model_name.empty()) {
    return tensorflow::errors::InvalidArgument("Missing model name in request.");
  }

  CreateTaskRequest request;
  TF_RETURN_IF_ERROR(unserializeMessage(request_body, &request));

  request.mutable_model_spec()->set_name(std::string(model_name));
  if (model_version.has_value()) {
    request.mutable_model_spec()->mutable_version()->set_value(model_version.value());
  }
  CreateTaskResponse response;
  TF_RETURN_IF_ERROR(toTensorflowStatus(task_op_impl.create(runOptions(options), request, response)));
  std::string response_output;
  TF_RETURN_IF_ERROR(serializeMessage(response, &response_output));
  absl::StrAppend(output, response_output);
  return tensorflow::Status::OK();
}

tensorflow::Status HttpRestApiHandler::toTensorflowStatus(const cub::StatusWrapper& status) {
  if (status.ok()) {
    return tensorflow::Status::OK();
  }
  return tensorflow::errors::Internal(status.error_message());
}

tensorflow::Status HttpRestApiHandler::processModelMetadataRequest(const absl::string_view model_name,
                                                                   const absl::string_view model_version_str,
                                                                   std::string* output) {
  GetModelMetaRequest request;
  if (model_name.empty()) {
    return tensorflow::errors::InvalidArgument("Missing model name in request.");
  }
  request.mutable_model_spec()->set_name(std::string(model_name));
  if (!model_version_str.empty()) {
    tensorflow::int64 version;
    if (!absl::SimpleAtoi(model_version_str, &version)) {
      return tensorflow::errors::InvalidArgument("Failed to convert version: ", model_version_str, " to numeric.");
    }
    request.mutable_model_spec()->mutable_version()->set_value(version);
  }

  GetModelMetaResponse response;
  TF_RETURN_IF_ERROR(get_model_meta_impl.getModelMeta(request, response));

  std::string response_output;
  TF_RETURN_IF_ERROR(serializeMessage(response, &response_output));
  absl::StrAppend(output, response_output);
  return tensorflow::Status::OK();
}

}  // namespace serving
}  // namespace adlik
