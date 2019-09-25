#include "adlik_serving/runtime/provider/predict_util.h"

namespace adlik {
namespace serving {

PredictUtil::PredictUtil(const RunOptions& opts, ModelHandle* handle, const PredictRequest& req, PredictResponse& rsp)
    : opts(opts), handle(handle), req(req), rsp(rsp) {
}

}  // namespace serving
}  // namespace adlik
