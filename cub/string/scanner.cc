#include "cub/string/scanner.h"

namespace cub {

Scanner::Scanner(StringView source) : cur(source) {
  restartCapture();
}

Scanner& Scanner::one(CharSpec spec) {
  if (cur.empty() || !spec(cur[0])) {
    return onError();
  }
  cur.removePrefix(1);
  return *this;
}

Scanner& Scanner::optional(StringView s) {
  cur.consumePrefix(s);
  return *this;
}

Scanner& Scanner::literal(StringView s) {
  return cur.consumePrefix(s) ? *this : onError();
}

Scanner& Scanner::any(CharSpec spec) {
  while (!cur.empty() && spec(cur[0])) {
    cur.removePrefix(1);
  }
  return *this;
}

Scanner& Scanner::many(CharSpec spec) {
  return one(spec).any(spec);
}

Scanner& Scanner::until(CharSpec spec) {
  any(is_not(spec));
  return cur.empty() ? onError() : *this;
}

Scanner& Scanner::restartCapture() {
  start = cur.data();
  end = nullptr;
  return *this;
}

Scanner& Scanner::stopCapture() {
  end = cur.data();
  return *this;
}

Scanner& Scanner::eos() {
  if (!cur.empty())
    error = true;
  return *this;
}

Scanner& Scanner::onError() {
  error = true;
  return *this;
}

char Scanner::peek(char defaultValue) const {
  return cur.empty() ? defaultValue : cur[0];
}

bool Scanner::empty() const {
  return cur.empty();
}

bool Scanner::result(StringView* remain, StringView* capture) {
  if (error) {
    return false;
  }
  if (remain != nullptr) {
    *remain = cur;
  }
  if (capture != nullptr) {
    auto last = (end == nullptr ? cur.data() : end);
    *capture = StringView(start, last - start);
  }
  return true;
}

}  // namespace cub
