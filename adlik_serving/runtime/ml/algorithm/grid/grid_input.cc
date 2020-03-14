#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"

namespace ml_runtime {

cub::StatusWrapper loadGridInput(const std::string& file_path, std::vector<GridInput>& inputs) {
  inputs.clear();
  return cub::StatusWrapper::OK();
}

}  // namespace ml_runtime
