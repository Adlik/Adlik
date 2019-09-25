#ifndef H320818E8_E608_4570_BE4C_721EF7A3194D
#define H320818E8_E608_4570_BE4C_721EF7A3194D

namespace cub {

// GCC on Linux only.

constexpr bool isLittleEndian() {
  return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
}

constexpr bool isGigEndian() {
  return __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;
}

}  // namespace cub

#endif
