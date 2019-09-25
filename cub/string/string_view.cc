#include "cub/string/string_view.h"
#include "algorithm"
#include "iostream"

namespace cub {

const size_t StringView::npos = -1;

StringView::StringView() : StringView(nullptr, 0) {
}

StringView::StringView(const std::string& s) : StringView(s.data(), s.size()) {
}

StringView::StringView(const char* s) : StringView(s, strlen(s)) {
}

StringView::StringView(const char* d, size_t n) : data_(d), size_(n) {
}

const char* StringView::data() const {
  return data_;
}

size_t StringView::size() const {
  return size_;
}

bool StringView::empty() const {
  return size_ == 0;
}

StringView::iterator StringView::begin() const {
  return data_;
}

StringView::iterator StringView::end() const {
  return data_ + size_;
}

char StringView::front() const {
  return data_[0];
}

char StringView::back() const {
  return data_[size_ - 1];
}

char StringView::operator[](size_t n) const {
  return data_[n];
}

std::string StringView::to_s() const {
  return std::string(*this);
}

StringView::operator std::string() const {
  return data() ? std::string(data(), size()) : std::string();
}

bool StringView::contains(StringView sub) const {
  return std::search(begin(), end(), sub.begin(), sub.end()) != end();
}

bool StringView::starts(StringView prefix) const {
  return prefix.empty() || (size() >= prefix.size() &&
                            memcmp(data(), prefix.data(), prefix.size()) == 0);
}

bool StringView::ends(StringView suffix) const {
  return suffix.empty() ||
         (size() >= suffix.size() && memcmp(
                                         data() + (size() - suffix.size()),
                                         suffix.data(),
                                         suffix.size()) == 0);
}

void StringView::removePrefix(size_t n) {
  data_ += n;
  size_ -= n;
}

void StringView::removeSuffix(size_t n) {
  size_ -= n;
}

template <typename Pred, typename Action>
bool StringView::consumeSubstr(StringView substr, Pred pred, Action action) {
  if ((this->*pred)(substr)) {
    (this->*action)(substr.size());
    return true;
  }
  return false;
}

bool StringView::consumePrefix(StringView prefix) {
  return consumeSubstr(prefix, &StringView::starts, &StringView::removePrefix);
}

bool StringView::consumeSuffix(StringView suffix) {
  return consumeSubstr(suffix, &StringView::ends, &StringView::removeSuffix);
}

template <typename F>
inline std::string StringView::map(F f) const {
  std::string result(data(), size());
  for (auto& c : result) {
    c = f(c);
  }
  return result;
}

std::string StringView::lower() const {
  return map([](char c) { return tolower(c); });
}

std::string StringView::upper() const {
  return map([](char c) { return toupper(c); });
}

StringView& StringView::ltrim() {
  size_t count = 0;
  auto ptr = data();
  while (count < size() && isspace(*ptr)) {
    count++;
    ptr++;
  }
  removePrefix(count);
  return *this;
}

StringView& StringView::rtrim() {
  size_t count = 0;
  auto ptr = data() + size() - 1;
  while (count < size() && isspace(*ptr)) {
    ++count;
    --ptr;
  }
  removeSuffix(count);
  return *this;
}

StringView& StringView::trim() {
  ltrim();
  return rtrim();
}

void StringView::split(StringView delims, std::vector<std::string>& result)
    const {
  if (empty())
    return;
  for (size_t start = 0, i = 0; i < size() + 1; ++i) {
    if ((i == size()) || (delims.find(data_[i]) != StringView::npos)) {
      result.push_back(std::string(data() + start, i - start));
      start = i + 1;
    }
  }
}

std::ostream& operator<<(std::ostream& o, StringView piece) {
  o.write(piece.data(), piece.size());
  return o;
}

size_t StringView::find(char c, size_t pos) const {
  if (pos >= size_) {
    return npos;
  }
  auto result = static_cast<const char*>(memchr(data_ + pos, c, size_ - pos));
  return result != nullptr ? result - data_ : npos;
}

size_t StringView::rfind(char c, size_t pos) const {
  if (size_ == 0)
    return npos;
  for (auto p = data_ + std::min(pos, size_ - 1); p >= data_; p--) {
    if (*p == c) {
      return p - data_;
    }
  }
  return npos;
}

StringView StringView::substr(size_t pos, size_t n) const {
  if (pos > size_)
    pos = size_;
  if (n > size_ - pos)
    n = size_ - pos;
  return {data_ + pos, n};
}

inline int StringView::compare(StringView b) const {
  auto min = (size_ < b.size_) ? size_ : b.size_;
  auto r = memcmp(data_, b.data_, min);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = 1;
  }
  return r;
}

bool operator==(StringView x, StringView y) {
  return x.size() == y.size() && memcmp(x.data(), y.data(), x.size()) == 0;
}

bool operator!=(StringView x, StringView y) {
  return !(x == y);
}

bool operator<(StringView x, StringView y) {
  return x.compare(y) < 0;
}

bool operator>(StringView x, StringView y) {
  return x.compare(y) > 0;
}

bool operator<=(StringView x, StringView y) {
  return x.compare(y) <= 0;
}

bool operator>=(StringView x, StringView y) {
  return x.compare(y) >= 0;
}

}  // namespace cub
