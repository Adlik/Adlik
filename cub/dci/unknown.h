#ifndef H05B2224D_B926_4FC0_A936_67B52B8A98DF
#define H05B2224D_B926_4FC0_A936_67B52B8A98DF

#include "cub/dci/interface_def.h"

namespace com {
UNKNOWN_INTERFACE(Unknown, 0xFFFFFFFE) {
  virtual void* castTo(::details::InterfaceId iid) const = 0;
};

template <typename TO>
TO* unknown_cast(const Unknown* from) {
  return from == 0 ? 0 : (TO*)from->castTo(TO::E_IID);
}
}  // namespace com

//////////////////////////////////////////////////////////////////////////
#define BEGIN_INTERFACE_TABLE()                              \
  OVERRIDE(void* castTo(::details::InterfaceId iid) const) { \
    switch (iid) {
#define END_INTERFACE_TABLE() \
  case 0xFFFFFFFF:            \
  default:                    \
    return nullptr;           \
    }                         \
    }

//////////////////////////////////////////////////////////////////////////
#define BEGIN_OVERRIDE_INTERFACE_TABLE(super)                \
  OVERRIDE(void* castTo(::details::InterfaceId iid) const) { \
    void* t = super::castTo(iid);                            \
    if (t != nullptr)                                        \
      return t;                                              \
    switch (iid) {
#define BEGIN_OVERRIDE_INTERFACE_TABLE_2(super1, super2)     \
  OVERRIDE(void* castTo(::details::InterfaceId iid) const) { \
    void* t = super1::castTo(iid);                           \
    if (t != nullptr)                                        \
      return t;                                              \
    t = super2::castTo(iid);                                 \
    if (t != nullptr)                                        \
      return t;                                              \
    switch (iid) {
#define END_OVERRIDE_INTERFACE_TABLE(super) END_INTERFACE_TABLE()

#define __HAS_INTERFACE(iface) \
  case iface::E_IID:           \
    return const_cast<iface*>(static_cast<const iface*>(this));

#define APPEND_INTERFACE_TABLE(parent)                       \
  OVERRIDE(void* castTo(::details::InterfaceId iid) const) { \
    void* p = parent::castTo(iid);                           \
    if (p != nullptr)                                        \
      return p;                                              \
    switch (iid) {
//////////////////////////////////////////////////////////////////////////
#define DECL_CONTEXT()                                 \
private:                                               \
  const ::com::Unknown* context;                       \
                                                       \
public:                                                \
  void setUserContext(const ::com::Unknown* context) { \
    this->context = context;                           \
  }                                                    \
  const ::com::Unknown* getUserContext() const {       \
    return context;                                    \
  }

#define __IMPL_CONTEXT_ROLE(role) return *role;

#define IMPL_CONTEXT_ROLE(object, role)                     \
  DECL_OBJECT_ROLE(object, role) {                          \
    __IMPL_CONTEXT_ROLE(::com::unknown_cast<role>(context)) \
  }

#define DEF_SIMPLE_CONTEXT(context, super)           \
  struct context : ::com::Unknown, protected super { \
    BEGIN_OVERRIDE_INTERFACE_TABLE(super)            \
    END_INTERFACE_TABLE()                            \
  }

#endif
