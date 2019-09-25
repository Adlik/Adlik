#ifndef H1C6C67BC_6CDE_422A_B3F7_9B7D0B546E8C
#define H1C6C67BC_6CDE_422A_B3F7_9B7D0B546E8C

#include "cub/string/string_view.h"

namespace cub {

struct Uri {
  Uri(StringView uri);
  Uri(StringView scheme, StringView host, StringView path);

  void get(StringView& scheme, StringView& host, StringView& path) const;
  std::string to_s() const;

private:
  StringView scheme, host, path;
};

}  // namespace cub

#endif
