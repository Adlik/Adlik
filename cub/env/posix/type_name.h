// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H9FBC04FF_86B1_4F8A_8EAC_54C7E689976C
#define H9FBC04FF_86B1_4F8A_8EAC_54C7E689976C

#include <string>
#include <typeinfo>

namespace cub {

std::string demangle(const std::type_info&);

template <typename T>
struct TypeName {
  static std::string value() {
    return demangle(typeid(T));
  }
};

template <>
struct TypeName<std::string> {
  static std::string value() {
    return "std::string";
  }
};

template <>
struct TypeName<std::nullptr_t> {
  static std::string value() {
    return "std::nullptr_t";
  }
};

}  // namespace cub

#endif
