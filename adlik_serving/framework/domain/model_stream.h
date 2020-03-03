// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HC366A848_840F_4E47_8EB2_CB9402478873
#define HC366A848_840F_4E47_8EB2_CB9402478873

#include <string>

#include "adlik_serving/framework/domain/version_list.h"

namespace adlik {
namespace serving {

struct ModelId;
struct Model;

struct ModelStream {
  ModelStream(const std::string& name, const VersionList&);

  bool matches(const std::string& name) const;
  bool contains(const std::string& name, int version) const;
  bool contains(int version) const;

  const std::string& getName() const;
  std::string str() const;

  template <typename F>
  void versions(F f) const {
    list.list(f);
  }

private:
  std::string name;
  VersionList list;
};

}  // namespace serving
}  // namespace adlik

#endif
