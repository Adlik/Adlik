#include "cub/env/posix/type_name.h"
#include <cxxabi.h>
#include <cstdlib>
#include <memory>

namespace cub {

std::string demangle(const std::type_info& info) {
  auto status = 0;
  std::unique_ptr<char, void (*)(void*)> res{
      abi::__cxa_demangle(info.name(), nullptr, nullptr, &status), std::free};
  return status == 0 ? res.get() : info.name();
}

}  // namespace cub
