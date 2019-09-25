#ifndef H05B2224D_B926_4FC0_A936_97B52B8A98DB
#define H05B2224D_B926_4FC0_A936_97B52B8A98DB

#include "cub/base/default.h"

namespace cub {

namespace details {
template <typename T>
struct Interface {
  virtual ~Interface() {
  }
};
}  // namespace details

#define DEF_INTERFACE(type) struct type : ::cub::details::Interface<type>

#define ABSTRACT(...) virtual __VA_ARGS__ = 0
#define OVERRIDE(...) __VA_ARGS__ override
#define FINAL(...) __VA_ARGS__ final

#define EXTENDS(...) , ##__VA_ARGS__
#define IMPLEMENTS(...) EXTENDS(__VA_ARGS__)

}  // namespace cub

#endif
