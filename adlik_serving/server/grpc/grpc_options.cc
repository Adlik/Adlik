// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/grpc/grpc_options.h"

#include "adlik_serving/server/grpc/ssl_config.pb.h"
#include "cub/cli/program_options.h"
#include "cub/log/log.h"
#include "cub/protobuf/text_protobuf.h"
#include "cub/string/str_utils.h"
#include "grpcpp/security/server_credentials.h"
#include "grpcpp/server_builder.h"

namespace adlik {
namespace serving {

namespace {
struct ChannelBuilder {
  ChannelBuilder(cub::StringView args) {
    for (cub::StringView sp : cub::strutils::split(args, ",")) {
      auto&& kv = cub::strutils::split(sp.trim(), "=");
      channels.insert({kv[0], kv[1]});
    }
  }

  void build(grpc::ServerBuilder& builder) const {
    for (auto& c : channels) {
      builder.AddChannelArgument(c.first, c.second);
    }
  }

private:
  std::map<std::string, std::string> channels;
};

inline auto getRequestType(const SSLConfig& config) {
  return config.client_verify() ? GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY :
                                  GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE;
}

inline auto getSecureCredentials(const SSLConfig& config) {
  grpc::SslServerCredentialsOptions opts;

  opts.client_certificate_request = getRequestType(config);
  opts.force_client_auth = config.client_verify();

  if (config.custom_ca().size() > 0) {
    opts.pem_root_certs = config.custom_ca();
  }

  opts.pem_key_cert_pairs.push_back({config.server_key(), config.server_cert()});

  return grpc::SslServerCredentials(opts);
}

inline SSLConfig sslConfig(const std::string& file) {
  SSLConfig config;
  CUB_PEEK_TRUE(cub::TextProtobuf(file).parse(config));
  return config;
}

inline auto credentials(const std::string& sslFile) {
  if (sslFile.empty()) {
    return grpc::InsecureServerCredentials();
  } else {
    return getSecureCredentials(sslConfig(sslFile));
  }
}

inline std::string address(int port) {
  return std::string("0.0.0.0:") + std::to_string(port);
}
}  // namespace

GrpcOptions::GrpcOptions() : port(0) {
}

void GrpcOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("grpc_port", &port, "Port to listen on for gRPC API"),
      cub::option("ssl_config_file", &sslFile, "ssl protobuf dfile"),
      cub::option("grpc_channel_args", &channels, "A comma separated list of arguments to be passed to "),
  }};
  return prog.add(options);
}

bool GrpcOptions::build(::grpc::ServerBuilder& builder) {
  if (port > 0) {
    INFO_LOG << "grpc server port: " << port;
    builder.SetMaxMessageSize(INT32_MAX);
    builder.AddListeningPort(address(port), credentials(sslFile));
    ChannelBuilder(channels).build(builder);
    return true;
  } else {
    INFO_LOG << "grpc port is " << port << ", can't start grpc server";
    return false;
  }
}

}  // namespace serving
}  // namespace adlik
