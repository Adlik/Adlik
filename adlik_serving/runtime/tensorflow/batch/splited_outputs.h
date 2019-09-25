#ifndef HFA4E2C2C_EFEA_403F_A4A8_DF9616C548F2
#define HFA4E2C2C_EFEA_403F_A4A8_DF9616C548F2

#include "adlik_serving/runtime/tensorflow/batch/splited_tensors.h"
#include "adlik_serving/runtime/tensorflow/model/model_outputs.h"

namespace tensorflow {

struct SplitedSizes;
struct ModelSignature;

struct SplitedOuputs {
  SplitedOuputs(SplitedSizes& sizes);

  Status split(const OutputNames&, const OutputTensors&);
  const SplitedTensors* find(const std::string& name) const;

private:
  Status split(const std::string&, const Tensor&);

private:
  SplitedSizes& sizes;
  std::map<std::string, SplitedTensors> tensors;
};

}  // namespace tensorflow

#endif
