#include "cub/env/fs/uri.h"
#include "cub/string/scanner.h"

namespace cub {

namespace {
bool parseScheme(StringView& remain, StringView& scheme) {
  return Scanner(remain)
      .one(alpha())
      .many(is_or(alnum(), dot()))
      .stopCapture()
      .literal("://")
      .result(&remain, &scheme);
}

bool parseHost(StringView& remain, StringView& host) {
  return Scanner(remain).until(ch('/')).result(&remain, &host);
}

void parse(
    StringView remain,
    StringView& scheme,
    StringView& host,
    StringView& path) {
  if (!parseScheme(remain, scheme)) {
    scheme = StringView(remain.begin(), 0);
    host = StringView(remain.begin(), 0);
    path = remain;
    return;
  }

  if (!parseHost(remain, host)) {
    host = remain;
    path = StringView(remain.end(), 0);
    return;
  }

  path = remain;
}
}  // namespace

Uri::Uri(StringView uri) {
  parse(uri, scheme, host, path);
}

Uri::Uri(StringView scheme, StringView host, StringView path)
  : scheme(scheme), host(host), path(path) {
}

void Uri::get(StringView& scheme, StringView& host, StringView& path) const {
  scheme = this->scheme;
  host = this->host;
  path = this->path;
}

std::string Uri::to_s() const {
  if (scheme.empty()) {
    return std::string(path);
  }
  return std::string(scheme) + "://" + std::string(host) + std::string(path);
}

}  // namespace cub
