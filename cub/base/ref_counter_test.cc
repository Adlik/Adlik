#include "cub/base/ref_counter.h"
#include "cub/base/scoped_unref.h"
#include "cut/cut.hpp"

namespace cub {

namespace {
  int constructed = 0;
  int destroyed = 0;

  struct MyRef : RefCounter {
    MyRef() { constructed++; }
    ~MyRef() override { destroyed++; }
  };
}

FIXTURE(RefTest) {
  SETUP() {
    constructed = 0;
    destroyed = 0;
  }

  TEST("New") {
    MyRef* ref = new MyRef;
    ASSERT_EQ(1, constructed);
    ASSERT_EQ(0, destroyed);
    ref->unref();
    ASSERT_EQ(1, constructed);
    ASSERT_EQ(1, destroyed);
  }

  TEST("Refunref") {
    MyRef* ref = new MyRef;
    ASSERT_EQ(1, constructed);
    ASSERT_EQ(0, destroyed);
    ref->ref();
    ASSERT_EQ(0, destroyed);
    ref->unref();
    ASSERT_EQ(0, destroyed);
    ref->unref();
    ASSERT_EQ(1, destroyed);
  }

  TEST("RefCountOne") {
    MyRef* ref = new MyRef;
    ASSERT_TRUE(ref->exactlyOne());
    ref->unref();
  }

  TEST("RefCountNotOne") {
    MyRef* ref = new MyRef;
    ref->ref();
    ASSERT_FALSE(ref->exactlyOne());
    ref->unref();
    ref->unref();
  }

  TEST("ConstRefunref") {
    const MyRef* cref = new MyRef;
    ASSERT_EQ(1, constructed);
    ASSERT_EQ(0, destroyed);
    cref->ref();
    ASSERT_EQ(0, destroyed);
    cref->unref();
    ASSERT_EQ(0, destroyed);
    cref->unref();
    ASSERT_EQ(1, destroyed);
  }

  TEST("ReturnOfunref") {
    MyRef* ref = new MyRef;
    ref->ref();
    ASSERT_FALSE(ref->unref());
    ASSERT_TRUE(ref->unref());
  }

  TEST("Scopedunref") {
    { ScopedUnref unref(new MyRef); }
    ASSERT_EQ(destroyed, 1);
  }

  TEST("Scopedunref_Nullptr") {
    { ScopedUnref unref(nullptr); }
    ASSERT_EQ(destroyed, 0);
  }
};

}
