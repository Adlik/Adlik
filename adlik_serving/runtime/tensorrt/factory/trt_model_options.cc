#include "adlik_serving/runtime/tensorrt/factory/trt_model_options.h"

#include "cub/cli/program_options.h"

namespace tensorrt {

TrtModelOptions::TrtModelOptions() : enableBatching(false) {
}

void TrtModelOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("trt_enable_batching", &enableBatching, "enable trt batching"),
  }};
  prog.add(options);
}

}  // namespace tensorrt
