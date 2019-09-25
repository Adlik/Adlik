#include "cub/log/color.h"

namespace cub {

#define __white "\033[0m"
#define __red "\033[1;31m"
#define __green "\033[1;32m"
#define __yellow "\033[1;33m"
#define __blue "\033[1;34m"
#define __magenta "\033[1;35m"
#define __cyan "\033[1;36m"

#define DEF_COLOR(color)                  \
  std::ostream& color(std::ostream& os) { \
    return os << __##color;               \
  }

DEF_COLOR(white)
DEF_COLOR(red)
DEF_COLOR(green)
DEF_COLOR(yellow)
DEF_COLOR(blue)
DEF_COLOR(magenta)
DEF_COLOR(cyan)

}  // namespace cub
