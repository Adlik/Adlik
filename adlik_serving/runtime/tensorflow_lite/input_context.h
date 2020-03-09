#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H

#include <cstdlib>

namespace adlik {
namespace serving {
struct InputContext {
  int tensorIndex;
  mutable size_t bytesWrittenCache = 0;

  explicit InputContext(int tensorIndex) : tensorIndex(tensorIndex) {
  }
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
