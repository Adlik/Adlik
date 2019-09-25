#ifndef H05B2224D_B926_4FC0_A936_87B52B8A98DB
#define H05B2224D_B926_4FC0_A936_87B52B8A98DB

#include "cub/base/keywords.h"

#define ROLE(role) get##role()
#define ROLE_PROTO_TYPE(role) role& ROLE(role) const
#define USE_ROLE(role) virtual ROLE_PROTO_TYPE(role) = 0
#define USE_ROLE_NS(ns, role) virtual ns::role& ROLE(role) const = 0
#define HAS_ROLE(role) USE_ROLE(role)

#define IMPL_ROLE_FROM_THIS(role) const_cast<role&>(static_cast<const role&>(*this))

#define IMPL_ROLE_FROM_THIS_NS(ns, role) const_cast<ns::role&>(static_cast<const ns::role&>(*this))

//////////////////////////////////////////////////////////////////
#define IMPL_ROLE(role)               \
  ROLE_PROTO_TYPE(role) override {    \
    return IMPL_ROLE_FROM_THIS(role); \
  }

#define IMPL_ROLE_NS(ns, role)               \
  ns::role& ROLE(role) const override {      \
    return IMPL_ROLE_FROM_THIS_NS(ns, role); \
  }

//////////////////////////////////////////////////////////////////
#define IMPL_ROLE_WITH_TYPE(role, type) \
  ROLE_PROTO_TYPE(role) override {      \
    return IMPL_ROLE_FROM_THIS(type);   \
  }

#define __TYPE_VAR(type) type##var

//////////////////////////////////////////////////////////////////
#define IMPL_ROLE_WITH_VAR(role, type)          \
private:                                        \
  ROLE_PROTO_TYPE(role) override {              \
    return const_cast<type&>(__TYPE_VAR(type)); \
  }                                             \
  type __TYPE_VAR(type)

#define __ROLE_VAR(role) role##var

//////////////////////////////////////////////////////////////////
#define IMPL_ROLE_WITH_ROLE_VAR(role)    \
private:                                 \
  ROLE_PROTO_TYPE(role) override {       \
    return const_cast<role&>(role##var); \
  }                                      \
  role __ROLE_VAR(role)

//////////////////////////////////////////////////////////////////////////
#define ROLE_PTR(RoleType) RoleType##__
#define SET_ROLE(RoleType) set##RoleType

#define DECL_ROLE(role) ROLE_PROTO_TYPE(role)

#define EXPORT_ROLE(role) USE_ROLE(role)

#define __ROLE_CAST_TO(role, obj) return const_cast<role&>(static_cast<const role&>(obj))

//////////////////////////////////////////////////////////////////////////
#define IMPL_ROLE_WITH_OBJ(role, obj) \
  DECL_ROLE(role) {                   \
    __ROLE_CAST_TO(role, obj);        \
  }

//////////////////////////////////////////////////////////////////////////
#define DECL_DEPENDENCY(RoleType)                        \
public:                                                  \
  void SET_ROLE(RoleType)(RoleType & role) {             \
    ROLE_PTR(RoleType) = &role;                          \
  }                                                      \
  DECL_ROLE(RoleType) {                                  \
    return *(const_cast<RoleType*>(ROLE_PTR(RoleType))); \
  }                                                      \
                                                         \
private:                                                 \
  RoleType* ROLE_PTR(RoleType)

//////////////////////////////////////////////////////////////////////////
#define DECL_OBJECT_ROLE(Object, RoleType) RoleType& Object::get##RoleType() const

/////////////////////////////////////////////////////////////////////////////
namespace dci {
template <typename T>
struct Role {
  virtual ~Role() {
  }
};
}  // namespace dci

#define DEFINE_ROLE(role) struct role : ::dci::Role<role>

#define UPCAST(self, role) static_cast<role&>(self)
#define UPCAST_P(self, role) UPCAST(*self, role)
#define UPCAST_CONST(self, role) static_cast<const role&>(self)

#define SELF(role) static_cast<role&>(*this)
#define SELF_CONST(role) static_cast<const role&>(*this)

#endif
