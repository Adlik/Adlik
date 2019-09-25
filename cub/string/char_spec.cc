#include "cub/string/char_spec.h"
#include <ctype.h>

namespace cub {

namespace {
template <bool expected>
CharSpec expect() {
  return [](char) { return expected; };
}
}  // namespace

CharSpec always() {
  return expect<true>();
}

CharSpec never() {
  return expect<false>();
}

CharSpec atom(CType f) {
  return [f](char c) { return f(c) != 0; };
}

CharSpec ch(char c1) {
  return [c1](char c2) { return c1 == c2; };
}

#define DEF_CTYPE_SPEC(name) \
  CharSpec name() {          \
    return atom(::is##name); \
  }

DEF_CTYPE_SPEC(alnum)
DEF_CTYPE_SPEC(alpha)
DEF_CTYPE_SPEC(cntrl)
DEF_CTYPE_SPEC(digit)
DEF_CTYPE_SPEC(lower)
DEF_CTYPE_SPEC(graph)
DEF_CTYPE_SPEC(print)
DEF_CTYPE_SPEC(punct)
DEF_CTYPE_SPEC(space)
DEF_CTYPE_SPEC(upper)
DEF_CTYPE_SPEC(xdigit)

#define DEF_CH_SPEC(name, c) \
  CharSpec name() {          \
    return ch(c);            \
  }

DEF_CH_SPEC(dot, '.')
DEF_CH_SPEC(dash, '-')
DEF_CH_SPEC(plus, '+')
DEF_CH_SPEC(minus, '-')
DEF_CH_SPEC(zero, '0')
DEF_CH_SPEC(slash, '/')
DEF_CH_SPEC(underscore, '_')
DEF_CH_SPEC(escaped, '\\')

namespace internal {
namespace {
template <bool expected>
CharSpec shortcut(const std::vector<CharSpec>& specs) {
  return [specs](char c) {
    for (auto& spec : specs) {
      if (spec(c) == expected)
        return expected;
    }
    return !expected;
  };
}
}  // namespace

CharSpec is_and_impl(const std::vector<CharSpec>& specs) {
  return shortcut<false>(specs);
}

CharSpec is_or_impl(const std::vector<CharSpec>& specs) {
  return shortcut<true>(specs);
}

}  // namespace internal

CharSpec is_not(CharSpec spec) {
  return [spec](char c) { return !spec(c); };
}

}  // namespace cub
