#ifndef HCC269D7A_4829_4EA4_9E25_B896AB4DDFE7
#define HCC269D7A_4829_4EA4_9E25_B896AB4DDFE7

#include "adlik_serving/framework/manager/runtime_suite.h"
#include "cub/base/symbol.h"

namespace adlik {
namespace serving {

template <typename Runtime>
struct AutoRuntimeRegister {
  AutoRuntimeRegister(const char* name) {
    RuntimeSuite::inst().add(name, new Runtime(name));
  }
};

#define REGISTER_RUNTIME(RuntimeClass, RuntimeName) \
  static adlik::serving::AutoRuntimeRegister<RuntimeClass> UNIQUE_NAME(rt)(RuntimeName)

}  // namespace serving
}  // namespace adlik

#endif
