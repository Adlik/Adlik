#include "cub/env/fs/path.h"
#include <iostream>
#include "cub/env/fs/uri.h"

namespace cub {

Path::Path(StringView path) : path(std::string(path)) {
}

namespace {
bool isAbsolutePath(StringView path) {
  return !path.empty() && path[0] == '/';
}
}  // namespace

bool Path::isRelative() const {
  return !isAbsolute();
}

bool Path::isAbsolute() const {
  return isAbsolutePath(path);
}

StringView Path::fullPath() const {
  return path;
}

namespace {
std::pair<StringView, StringView> splitPath(StringView uri) {
  Uri parsed(uri);

  StringView scheme, host, path;
  parsed.get(scheme, host, path);

  auto pos = path.rfind('/');
  if (pos == StringView::npos) {
    return {{uri.begin(), size_t(host.end() - uri.begin())}, path};
  } else if (pos == 0) {
    return {{uri.begin(), size_t(path.begin() + 1 - uri.begin())},
            {path.data() + 1, path.size() - 1}};
  } else {
    return {{uri.begin(), size_t(path.begin() + pos - uri.begin())},
            {path.data() + pos + 1, path.size() - (pos + 1)}};
  }
}
}  // namespace

StringView Path::dirName() const {
  return splitPath(path).first;
}

StringView Path::baseName() const {
  return splitPath(path).second;
}

namespace {
std::pair<StringView, StringView> splitBasename(StringView path) {
  auto pos = path.rfind('.');
  if (pos == StringView::npos)
    return {path, {path.data() + path.size(), 0}};
  return {{path.data(), pos}, {path.data() + pos + 1, path.size() - (pos + 1)}};
}
}  // namespace

StringView Path::fileName() const {
  return splitBasename(baseName()).first;
}

StringView Path::extName() const {
  return splitBasename(baseName()).second;
}

std::string Path::normalize(StringView rhs) const {
  if (rhs.empty())
    return "";

  if (path.back() == '/') {
    if (isAbsolutePath(rhs)) {
      return std::string(rhs.substr(1));
    }
  }

  if (!isAbsolutePath(rhs)) {
    return std::string("/") + std::string(rhs);
  }

  return std::string(rhs);
}

Path& Path::join(StringView rhs) {
  path += (path.empty() ? std::string(rhs) : normalize(rhs));
  return *this;
}

namespace internal {
void joinPaths(std::initializer_list<StringView> paths, Path& result) {
  for (auto path : paths) {
    result.join(path);
  }
}
}  // namespace internal

}  // namespace cub
