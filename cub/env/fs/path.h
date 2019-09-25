#ifndef H67177F30_1107_45A0_A764_8E764C4693D7
#define H67177F30_1107_45A0_A764_8E764C4693D7

#include <initializer_list>
#include "cub/string/string_view.h"

namespace cub {

struct Path {
  Path(StringView path);

  bool isRelative() const;
  bool isAbsolute() const;

  // URI = dirName + baseName
  // baseName = fileName + extName
  StringView fullPath() const;
  StringView dirName() const;
  StringView baseName() const;
  StringView fileName() const;
  StringView extName() const;

  Path& join(StringView rhs);

private:
  std::string normalize(StringView rhs) const;

private:
  std::string path;
};

namespace internal {
void joinPaths(std::initializer_list<StringView> paths, Path& result);
}

template <typename... T>
std::string paths(const T&... args) {
  Path result("");
  internal::joinPaths({args...}, result);
  return std::string(result.fullPath());
}

}  // namespace cub

#endif
