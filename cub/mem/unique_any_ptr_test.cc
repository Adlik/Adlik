#include "cub/mem/unique_any_ptr.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(AnyPtrTest) {
  struct Destructable {
    Destructable(bool* destroyed) : destroyed(destroyed) {
    }

    ~Destructable() {
      *destroyed = true;
    }

    bool* const destroyed;
  };

  TEST("set && get && destroy") {
    bool destroyed = false;
    UniqueAnyPtr ptr;

    ptr = std::make_unique<Destructable>(&destroyed);
    ASSERT_THAT(ptr.get<Destructable>()->destroyed, eq(&destroyed));
    ASSERT_THAT(ptr.get<int>(), nil());
    ASSERT_FALSE(destroyed);

    ptr = nullptr;
    ASSERT_TRUE(destroyed);
  }

  TEST("move") {
    UniqueAnyPtr ptr1(std::make_unique<int>(1));
    UniqueAnyPtr ptr2(std::move(ptr1));
    ASSERT_THAT(*ptr2.get<int>(), eq(1));
  }
};

}
