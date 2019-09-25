#ifndef HACEBA5AA_27B2_403A_B1EA_AA8056B4F282
#define HACEBA5AA_27B2_403A_B1EA_AA8056B4F282

#include <functional>
#include <vector>

namespace cub {

using CharSpec = std::function<bool(char)>;

CharSpec always();
CharSpec never();

using CType = int (*)(int);

CharSpec atom(CType);
CharSpec ch(char c);

namespace internal {
CharSpec is_and_impl(const std::vector<CharSpec>&);
CharSpec is_or_impl(const std::vector<CharSpec>&);
}  // namespace internal

template <typename... T>
CharSpec is_and(const T&... specs) {
  return internal::is_and_impl({specs...});
}

template <typename... T>
CharSpec is_or(const T&... specs) {
  return internal::is_or_impl({specs...});
}

CharSpec is_not(CharSpec);

#define DECL_ATOM_SPEC(name) CharSpec name()

DECL_ATOM_SPEC(alnum);
DECL_ATOM_SPEC(alpha);
DECL_ATOM_SPEC(cntrl);
DECL_ATOM_SPEC(digit);
DECL_ATOM_SPEC(lower);
DECL_ATOM_SPEC(graph);
DECL_ATOM_SPEC(print);
DECL_ATOM_SPEC(punct);
DECL_ATOM_SPEC(space);
DECL_ATOM_SPEC(upper);
DECL_ATOM_SPEC(xdigit);

DECL_ATOM_SPEC(dot);
DECL_ATOM_SPEC(dash);
DECL_ATOM_SPEC(plus);
DECL_ATOM_SPEC(minus);
DECL_ATOM_SPEC(zero);
DECL_ATOM_SPEC(slash);
DECL_ATOM_SPEC(underscore);
DECL_ATOM_SPEC(escaped);

#undef DECL_ATOM_SPEC

}  // namespace cub

#endif
