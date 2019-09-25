#ifndef H77F84DB1_A8B6_4AEE_B353_C9358BB990A8
#define H77F84DB1_A8B6_4AEE_B353_C9358BB990A8

#include "cub/base/keywords.h"
#include "cub/string/string_view.h"

namespace cub {

DEF_INTERFACE(Option) {
  ABSTRACT(bool parse(StringView arg));
  ABSTRACT(const std::string& help() const);
};

// support long format only:
// 1. integer: --port=8005
// 2. boolean: --master=0  (0 is NO, non zero is YES)
// 3. float: --max=20.0
// 4. string: --note=something
Option* option(const char* name, int* dst, const char* usage);
Option* option(const char* name, float* dst, const char* usage);
Option* option(const char* name, bool* dst, const char* usage);
Option* option(const char* name, std::string* dst, const char* usage);

}  // namespace cub

#endif
