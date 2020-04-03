// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H49422F70_25E4_4BC5_B62E_49094085B51D
#define H49422F70_25E4_4BC5_B62E_49094085B51D

#include <initializer_list>
#include <utility>

#include "cub/base/inplace.h"
#include "cub/base/placement.h"

namespace cub {

struct NilOptional {
  explicit constexpr NilOptional(int) {
  }
};

extern const NilOptional nilopt;

template <typename T>
struct Optional {
  Optional() : present(false) {
  }

  Optional(NilOptional) : present(false) {
  }

  Optional(const Optional& src) {
    if (src) {
      construct(src.reference());
    }
  }

  Optional(Optional&& src) noexcept {
    if (src) {
      construct(std::move(src.reference()));
    }
  }

  Optional(const T& src) {
    construct(src);
  }

  Optional(T&& src) {
    construct(std::move(src));
  }

  template <typename... Args>
  explicit Optional(inplace_t, Args&&... args) {
    construct(std::forward<Args>(args)...);
  }

  // Optional<T>(in_place, {arg1, arg2, arg3}) -> T({arg1, arg2, arg3}).
  template <class U, typename... Args>
  explicit Optional(inplace_t, std::initializer_list<U> il, Args&&... args) {
    construct(il, std::forward<Args>(args)...);
  }

  ~Optional() {
    clear();
  }

  Optional& operator=(NilOptional) {
    clear();
    return *this;
  }

  Optional& operator=(const Optional& src) {
    if (src) {
      operator=(src.reference());
    } else {
      clear();
    }
    return *this;
  }

  Optional& operator=(Optional&& src) noexcept {
    if (src) {
      operator=(std::move(src.reference()));
    } else {
      clear();
    }
    return *this;
  }

  Optional& operator=(const T& src) {
    if (*this) {
      reference() = src;
    } else {
      construct(src);
    }
    return *this;
  }

  Optional& operator=(T&& src) {
    if (*this) {
      reference() = std::move(src);
    } else {
      construct(std::move(src));
    }
    return *this;
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    clear();
    construct(std::forward<Args>(args)...);
  }

  template <class U, class... Args>
  void emplace(std::initializer_list<U> il, Args&&... args) {
    clear();
    construct(il, std::forward<Args>(args)...);
  }

  void swap(Optional& src) {
    if (*this) {
      if (src) {
        using std::swap;
        swap(reference(), src.reference());
      } else {
        src.construct(std::move(reference()));
        destruct();
      }
    } else {
      if (src) {
        construct(std::move(src.reference()));
        src.destruct();
      }
    }
  }

  const T* operator->() const {
    return pointer();
  }

  T* operator->() {
    return pointer();
  }

  const T& operator*() const {
    return reference();
  }

  T& operator*() {
    return reference();
  }

  operator bool() const {
    return present;
  }

  const T& value() const& {
    return reference();
  }

  T& value() & {
    return reference();
  }

  T&& value() && {
    return std::move(reference());
  }

  const T&& value() const&& {
    return std::move(reference());
  }

  template <class U>
  T value_or(U&& val) const& {
    if (*this) {
      return reference();
    } else {
      return static_cast<T>(std::forward<U>(val));
    }
  }

  template <class U>
  T value_or(U&& val) && {
    if (*this) {
      return std::move(reference());
    } else {
      return static_cast<T>(std::forward<U>(val));
    }
  }

private:
  const T* pointer() const {
    return storage.getObject();
  }

  T* pointer() {
    return storage.getObject();
  }

  const T& reference() const {
    return *pointer();
  }

  T& reference() {
    return *pointer();
  }

  template <class... Args>
  void construct(Args&&... args) {
    present = true;
    new (pointer()) T(std::forward<Args>(args)...);
  }

  void destruct() {
    pointer()->T::~T();
    present = false;
  }

  void clear() {
    if (present) {
      destruct();
    }
  }

  bool present = false;
  Placement<T> storage;

  static_assert(!std::is_same<NilOptional, typename std::remove_cv<T>::type>::value,
                "Optional<nullopt_t> is not allowed.");
  static_assert(!std::is_same<inplace_t, typename std::remove_cv<T>::type>::value,
                "Optional<inplace_t> is not allowed.");
  static_assert(!std::is_reference<T>::value, "Optional<reference> is not allowed.");
};

template <typename T>
Optional<std::decay_t<T>> make_optional(T&& v) {
  return {std::forward<T>(v)};
}

template <typename T>
bool operator==(const Optional<T>& lhs, const Optional<T>& rhs) {
  if (!lhs && !rhs)
    return true;
  if (lhs && rhs)
    return *lhs == *rhs;
  return false;
}

template <typename T>
bool operator!=(const Optional<T>& lhs, const Optional<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
bool operator<(const Optional<T>& lhs, const Optional<T>& rhs) {
  if (!rhs) {
    return false;
  } else if (!lhs) {
    return true;
  } else {
    return *lhs < *rhs;
  }
}

template <typename T>
bool operator>(const Optional<T>& lhs, const Optional<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const Optional<T>& lhs, const Optional<T>& rhs) {
  return !(rhs < lhs);
}

template <typename T>
bool operator>=(const Optional<T>& lhs, const Optional<T>& rhs) {
  return !(lhs < rhs);
}

template <typename T>
bool operator==(const Optional<T>& lhs, NilOptional rhs) {
  return !lhs;
}

template <typename T>
bool operator==(NilOptional lhs, const Optional<T>& rhs) {
  return !rhs;
}

template <typename T>
bool operator!=(const Optional<T>& lhs, NilOptional rhs) {
  return static_cast<bool>(lhs);
}

template <typename T>
bool operator!=(NilOptional lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs);
}

template <typename T>
bool operator<(const Optional<T>& lhs, NilOptional rhs) {
  return false;
}

template <typename T>
bool operator<(NilOptional lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs);
}

template <typename T>
bool operator<=(const Optional<T>& lhs, NilOptional rhs) {
  return !lhs;
}

template <typename T>
bool operator<=(NilOptional lhs, const Optional<T>& rhs) {
  return true;
}

template <typename T>
bool operator>(const Optional<T>& lhs, NilOptional rhs) {
  return static_cast<bool>(lhs);
}

template <typename T>
bool operator>(NilOptional lhs, const Optional<T>& rhs) {
  return false;
}

template <typename T>
bool operator>=(const Optional<T>& lhs, NilOptional rhs) {
  return true;
}

template <typename T>
bool operator>=(NilOptional lhs, const Optional<T>& rhs) {
  return !rhs;
}

template <typename T>
bool operator==(const Optional<T>& lhs, const T& rhs) {
  return static_cast<bool>(lhs) ? *lhs == rhs : false;
}

template <typename T>
bool operator==(const T& lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs) ? lhs == *rhs : false;
}

template <typename T>
bool operator!=(const Optional<T>& lhs, const T& rhs) {
  return static_cast<bool>(lhs) ? !(*lhs == rhs) : true;
}

template <typename T>
bool operator!=(const T& lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs) ? !(lhs == *rhs) : true;
}

template <typename T>
bool operator<(const Optional<T>& lhs, const T& rhs) {
  return static_cast<bool>(lhs) ? *lhs < rhs : true;
}

template <typename T>
bool operator<(const T& lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs) ? lhs < *rhs : false;
}

template <typename T>
bool operator>(const Optional<T>& lhs, const T& rhs) {
  return static_cast<bool>(lhs) ? rhs < *lhs : false;
}

template <typename T>
bool operator>(const T& lhs, const Optional<T>& rhs) {
  return static_cast<bool>(rhs) ? *rhs < lhs : true;
}

template <typename T>
bool operator<=(const Optional<T>& lhs, const T& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator<=(const T& lhs, const Optional<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const Optional<T>& lhs, const T& rhs) {
  return !(lhs < rhs);
}

template <typename T>
bool operator>=(const T& lhs, const Optional<T>& rhs) {
  return !(lhs < rhs);
}

template <typename T>
inline void swap(Optional<T>& a, Optional<T>& b) {
  a.swap(b);
}

}  // namespace cub

#endif
