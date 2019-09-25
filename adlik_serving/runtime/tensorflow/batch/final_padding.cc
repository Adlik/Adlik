#include "adlik_serving/runtime/tensorflow/batch/final_padding.h"

namespace tensorflow {

FinalPadding& FinalPadding::nil() {
  static FinalPadding inst;
  return inst;
}

}  // namespace tensorflow
