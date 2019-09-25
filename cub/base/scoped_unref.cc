#include "cub/base/scoped_unref.h"
#include "cub/base/ref_counter.h"

namespace cub {

ScopedUnref::ScopedUnref(RefCounter* obj) : obj(obj) {
}

ScopedUnref::~ScopedUnref() {
  if (obj) {
    obj->unref();
  }
}

}  // namespace cub
