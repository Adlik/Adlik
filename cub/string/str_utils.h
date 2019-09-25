#ifndef HED06F7BD_D7BB_4F0C_A9E4_CB029148549A
#define HED06F7BD_D7BB_4F0C_A9E4_CB029148549A

#include "cub/string/string_view.h"

namespace cub {

namespace strutils {

size_t to_s(int32_t, char* buf);
size_t to_s(uint32_t, char* buf);

bool to_int32(StringView text, int32_t& value);
bool to_uint32(StringView text, uint32_t& value);

int32_t to_int32(StringView text);
uint32_t to_uint32(StringView text);

std::vector<std::string> split(StringView text, StringView delims);
std::string join(const std::vector<std::string>&, StringView sep);

}  // namespace strutils

}  // namespace cub

#endif
