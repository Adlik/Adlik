#include "cub/env/posix/library_loader.h"
#include <dlfcn.h>

namespace cub {

LibraryLoader::LibraryLoader(const char* filename)
  : handle(dlopen(filename, RTLD_NOW | RTLD_LOCAL)) {
}

void* LibraryLoader::getSymbol(const char* symbol) const {
  if (handle == nullptr) {
    return nullptr;
  } else if (auto result = dlsym(handle, symbol)) {
    return result;
  } else {
    return nullptr;
  }
}

}  // namespace cub
