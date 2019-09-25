#include "cub/cli/option.h"
#include "cub/string/scanner.h"
#include "cub/string/str_printf.h"
#include "cub/string/str_utils.h"

#include <stdio.h>
#include <functional>
#include <iostream>

namespace cub {

namespace {
using Saver = std::function<bool(StringView)>;

struct OptionImpl : Option {
  OptionImpl(const char* name, Saver saver, std::string&& usage)
    : name(name), saver(std::move(saver)), usage(std::move(usage)) {
  }

private:
  OVERRIDE(bool parse(StringView arg)) {
    StringView rhs;
    return Scanner(arg).literal("--").literal(name).literal("=").result(&rhs) &&
           saver(rhs);
  }

  OVERRIDE(const std::string& help() const) {
    return usage;
  }

private:
  const char* name;
  Saver saver;
  std::string usage;
};

template <typename T, typename Parsed = T>
Saver saver(T* dst, const char* format) {
  return [dst, format](StringView value) {
    Parsed parsed;
    if (sscanf(value.data(), format, &parsed) == 1) {
      *dst = parsed;
      return true;
    }
    return false;
  };
}

template <typename T>
std::string format(T* t) {
  return std::to_string(*t);
}

template <>
std::string format(std::string* t) {
  using namespace std::string_literals;
  return "\""s + *t + "\""s;
}

std::string help(
    const char* name,
    const char* type,
    std::string&& init,
    const char* usage) {
  auto flag = stringprintf("--%s=%s", name, init.c_str());
  return stringprintf("  %-20s  %-12s  %s\n", flag.c_str(), type, usage);
}
}  // namespace

#define OPTION(type, saver) \
  new OptionImpl(name, saver, help(name, #type, format(dst), usage))

Option* option(const char* name, int* dst, const char* usage) {
  return OPTION(int, saver(dst, "%d"));
}

Option* option(const char* name, float* dst, const char* usage) {
  return OPTION(float, saver(dst, "%f"));
}

Option* option(const char* name, bool* dst, const char* usage) {
  return OPTION(bool, (saver<bool, int>(dst, "%d")));
}

Option* option(const char* name, std::string* dst, const char* usage) {
  Saver saver = [dst](StringView value) {
    *dst = std::string(value);
    return true;
  };
  return OPTION(std::string, saver);
}

}  // namespace cub
