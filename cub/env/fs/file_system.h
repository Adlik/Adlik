#ifndef HE95125AC_BE16_4B57_9EDB_799C5FCA7CAF
#define HE95125AC_BE16_4B57_9EDB_799C5FCA7CAF

#include <stdint.h>
#include <string>

#include "cub/base/status.h"
#include "cub/dci/role.h"
#include "cub/env/env.h"

namespace cub {

DEFINE_ROLE(ReadOnlyRegion) {
  ABSTRACT(const void* data() const);
  ABSTRACT(uint64_t size() const);

  std::string buff() const {
    return std::string(chars(), size());
  }

private:
  const char* chars() const {
    return static_cast<const char*>(data());
  }
};

DEFINE_ROLE(DirentVisitor) {
  ABSTRACT(void visit(const std::string& base, const std::string& child));
};

DEFINE_ROLE(FileSystem) {
  ABSTRACT(ReadOnlyRegion * mmap(const std::string&) const);
  ABSTRACT(Status children(const std::string&, DirentVisitor&) const);
  ABSTRACT(bool exists(const std::string&) const);
};

inline FileSystem& filesystem() {
  return Env::inst().ROLE(FileSystem);
}

}  // namespace cub

#endif
