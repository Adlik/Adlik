#ifndef H8EDC2955_65AA_4522_AAFE_E4BD6E869053
#define H8EDC2955_65AA_4522_AAFE_E4BD6E869053

#include <string>
#include <vector>

#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

using OutputNames = std::vector<std::string>;
using TargetNames = std::vector<std::string>;
using OutputAliases = std::vector<std::string>;
using OutputTensors = std::vector<Tensor>;

}  // namespace tensorflow

#endif
