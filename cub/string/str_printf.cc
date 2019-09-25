#include "cub/string/str_printf.h"

namespace cub {

namespace {
constexpr auto MAX_BUFF_SIZE = 1024;

void appendv(std::string& dst, const char* format, va_list va) {
  char buff[MAX_BUFF_SIZE];
  auto result = vsnprintf(buff, MAX_BUFF_SIZE, format, va);
  if (result >= 0 && result < MAX_BUFF_SIZE) {
    dst.append(buff, result);
  }
}
}  // namespace

void appendf(std::string& dst, const char* format, ...) {
  va_list va;
  va_start(va, format);
  appendv(dst, format, va);
  va_end(va);
}

std::string stringprintf(const char* format, ...) {
  va_list va;
  va_start(va, format);
  std::string result;
  appendv(result, format, va);
  va_end(va);
  return result;
}

}  // namespace cub
