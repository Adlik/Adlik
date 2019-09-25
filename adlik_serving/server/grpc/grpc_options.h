#ifndef H14108A40_DA18_4D67_B9E2_4985307DFF49
#define H14108A40_DA18_4D67_B9E2_4985307DFF49

#include <string>

#include "cub/base/fwddecl.h"
#include "cub/base/keywords.h"
#include "cub/cli/option_subscriber.h"

FWD_DECL_STRUCT(grpc, ServerBuilder)

namespace adlik {
namespace serving {

struct GrpcOptions : cub::OptionSubscriber {
  GrpcOptions();
  OVERRIDE(void subscribe(cub::ProgramOptions&));

  bool build(::grpc::ServerBuilder&);

private:
  int port;
  std::string channels;
  std::string sslFile;
};

}  // namespace serving
}  // namespace adlik

#endif
