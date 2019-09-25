#include "cub/string/str_utils.h"
#include <algorithm>

namespace cub {

namespace strutils {

namespace {
char safeFirstChar(StringView text) {
  if (text.empty())
    return '\0';
  return text[0];
}

bool startDigit(StringView text) {
  return ::isdigit(safeFirstChar(text));
}

void skipSpaces(StringView& text) {
  while (isspace(safeFirstChar(text))) {
    text.removePrefix(1);
  }
}

int signedFlag(StringView& text, int64_t& vmax) {
  int sign = 1;
  vmax = INT32_MAX;
  if (text.consumePrefix("-")) {
    sign = -1;
    ++vmax;
  }
  return sign;
}

int unsignedFlag(StringView&, int64_t& vmax) {
  vmax = UINT32_MAX;
  return 1;
}

int64_t toInt64(StringView& text, int64_t vmax) {
  int64_t result = 0;
  do {
    result = result * 10 + safeFirstChar(text) - '0';
    if (result > vmax) {
      return false;
    }
    text.removePrefix(1);
  } while (startDigit(text));
  return result;
}

template <typename To, typename F>
bool toInt(StringView text, To& value, F f) {
  skipSpaces(text);

  int64_t vmax = 0;
  auto sign = f(text, vmax);

  if (!startDigit(text))
    return false;

  auto result = toInt64(text, vmax);

  skipSpaces(text);

  if (!text.empty())
    return false;

  value = static_cast<To>(result * sign);
  return true;
}
}  // namespace

bool to_int32(StringView text, int32_t& value) {
  return toInt(text, value, signedFlag);
}

int32_t to_int32(StringView text) {
  int32_t n = 0;
  return to_int32(text, n) ? n : 0;
}

bool to_uint32(StringView text, uint32_t& value) {
  return toInt(text, value, unsignedFlag);
}

uint32_t to_uint32(StringView text) {
  uint32_t n = 0;
  return to_uint32(text, n) ? n : 0;
}

size_t to_s(int32_t i, char* buf) {
  uint32_t u = i;
  size_t length = 0;
  if (i < 0) {
    *buf++ = '-';
    ++length;
    u = 0 - u;
  }
  length += to_s(u, buf);
  return length;
}

size_t to_s(uint32_t i, char* buf) {
  char* start = buf;
  do {
    *buf++ = ((i % 10) + '0');
    i /= 10;
  } while (i > 0);
  *buf = 0;
  std::reverse(start, buf);
  return buf - start;
}

std::vector<std::string> split(StringView text, StringView delims) {
  std::vector<std::string> result;
  text.split(delims, result);
  return result;
}

std::string join(const std::vector<std::string>& strs, StringView sep) {
  std::string result;
  bool first = true;
  for (const auto& x : strs) {
    result += (first ? "" : sep.data());
    result += x;
    first = false;
  }
  return result;
}

}  // namespace strutils

}  // namespace cub
