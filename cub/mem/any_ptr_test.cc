#include "cub/mem/any_ptr.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(AnyPtrTest) {
  TEST("set && get") {
    AnyPtr ptr;
    int object;

    ptr = &object;
    ASSERT_THAT(ptr.get<int>(), eq(&object));
    ASSERT_THAT(ptr.get<bool>(), nil());

    ptr = nullptr;
    ASSERT_THAT(ptr.get<int>(), nil());
    ASSERT_THAT(ptr.get<bool>(), nil());
    ASSERT_THAT(ptr.get<void>(), nil());
    ASSERT_THAT(ptr.get<std::nullptr_t>(), nil());
  }

  TEST("const") {
    AnyPtr ptr;
    const int object = 0;
    ptr = &object;
    ASSERT_THAT(ptr.get<int>(), nil());
    ASSERT_THAT(ptr.get<const int>(), eq(&object));
  }

  TEST("base") {
    struct Base {
      virtual ~Base() {}
    private:
      int unused_base_var_ = 0;
    };

    struct Child : Base {
      ~Child() override {}
    private:
      int unused_child_var_ = 0;
    };

    AnyPtr ptr;
    Child c;
    ptr = &c;
    ASSERT_THAT(ptr.get<Base>(), nil());
    ASSERT_THAT(ptr.get<Child>(), eq(&c));

    ptr = static_cast<Base*>(&c);
    ASSERT_THAT(ptr.get<Base>(), eq(&c));
    ASSERT_THAT(ptr.get<Child>(), nil());
  }
};

}
