#ifndef RRTX_4775_BBGR_DRE2_BR64_BHDT0_12BVF
#define RRTX_4775_BBGR_DRE2_BR64_BHDT0_12BVF

#include "cub/string/char_spec.h"
#include "cub/string/str_utils.h"

namespace cub {

struct Scanner {
  explicit Scanner(StringView);

  Scanner& one(CharSpec);
  Scanner& optional(StringView);
  Scanner& literal(StringView);
  Scanner& any(CharSpec);
  Scanner& many(CharSpec);
  Scanner& until(CharSpec);

  Scanner& restartCapture();
  Scanner& stopCapture();
  Scanner& eos();

  char peek(char = '\0') const;
  bool empty() const;

  bool result(StringView* remain = nullptr, StringView* capture = nullptr);

private:
  void untilImpl(char end_ch, bool escaped);
  Scanner& onError();

private:
  StringView cur;
  const char* start = nullptr;
  const char* end = nullptr;
  bool error = false;
};

}  // namespace cub

#endif
