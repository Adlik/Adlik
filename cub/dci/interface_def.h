// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H05B2224D_B926_4FC0_A936_67B52B8A98DD
#define H05B2224D_B926_4FC0_A936_67B52B8A98DD

namespace details {
using InterfaceId = unsigned int;

struct Dummy {};

template <typename T_B1, typename T_B2, typename T_B3>
struct BaseTraits {
  struct Base : T_B1, T_B2, T_B3 {
    virtual ~Base() {
    }
  };
};

template <typename T_B1, typename T_B2>
struct BaseTraits<T_B1, T_B2, Dummy> {
  struct Base : T_B1, T_B2 {
    virtual ~Base() {
    }
  };
};

template <typename T_B1>
struct BaseTraits<T_B1, Dummy, Dummy> {
  typedef T_B1 Base;
};

template <>
struct BaseTraits<Dummy, Dummy, Dummy> {
  struct Base {
    virtual ~Base() {
    }
  };
};

template <InterfaceId I_IID, typename T_B1 = Dummy, typename T_B2 = Dummy, typename T_B3 = Dummy>
struct UnknownInterface : BaseTraits<T_B1, T_B2, T_B3>::Base {
  enum { E_IID = I_IID };
  virtual ~UnknownInterface() {
  }
};
}  // namespace details

#define UNKNOWN_INTERFACE(iface, iid, ...) struct iface : ::details::UnknownInterface<iid, ##__VA_ARGS__>

#endif
