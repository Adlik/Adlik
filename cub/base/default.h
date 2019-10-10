// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H05B2224D_B926_4FC0_A936_77B52B8A98DB
#define H05B2224D_B926_4FC0_A936_77B52B8A98DB

namespace cub {

namespace details {
template <typename T>
struct DefaultValue {
  static T value() {
    return T();
  }
};

template <typename T>
struct DefaultValue<T*> {
  static T* value() {
    return 0;
  }
};

template <typename T>
struct DefaultValue<const T*> {
  static T* value() {
    return 0;
  }
};

template <>
struct DefaultValue<void> {
  static void value() {
  }
};
}  // namespace details

#define DEFAULT(type, method)                           \
  virtual type method {                                 \
    return ::cub::details::DefaultValue<type>::value(); \
  }

}  // namespace cub

#endif
