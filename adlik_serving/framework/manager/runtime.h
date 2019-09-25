#ifndef HB0E954F2_8763_4C76_A5C7_DE904CDE9E2D
#define HB0E954F2_8763_4C76_A5C7_DE904CDE9E2D

#include "cub/base/fwddecl.h"
#include "cub/base/status.h"
#include "cub/cli/program_options.h"
#include "cub/dci/role.h"

FWD_DECL_STRUCT(tensorflow, Status)

namespace adlik {
namespace serving {

struct RuntimeContext;
struct PredictRequest;
struct PredictResponse;
struct ModelHandle;
struct RunOptions;

DEFINE_ROLE(Runtime) {
  /// \brief Subscribe program options.
  /// This API should be implemented in subclass if subclass care about command line
  /// parameters
  virtual void init(cub::ProgramOptions&) {
  }

  /// \brief Do something before creating model, e.g. register model facotory to
  /// FacotrySuite in this funciton
  ABSTRACT(cub::Status config(const RuntimeContext&));

  /// \brief Model prediction implemention.
  /// Note: ModelHandle can convert to custom model and do prediction, e.g.
  /// adlik::serving::AutoModelHandle<CustomModel> custom_model(handle);
  /// return custom_model.predict(req, rsp);
  virtual tensorflow::Status predict(const RunOptions&, ModelHandle*, const PredictRequest&, PredictResponse&);
};

}  // namespace serving
}  // namespace adlik

#endif
