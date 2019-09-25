#include "cub/base/struct_wrapper.h"
#include "cut/cut.hpp"

using namespace cum;
using namespace cub;

namespace {
  struct PlainMsg {
    uint32_t id;
    uint32_t transNum;
  };

  STRUCT_WRAPPER(DomainEvent, PlainMsg) {
    enum {
      MIN_ID = 0, MAX_ID = 20,
    };

    bool isValid() const {
      return (MIN_ID <= id) && (id <= MAX_ID);
    }
  };
}

FIXTURE(StructWrapperTest) {
  TEST("should use the method when wrappered") {
    PlainMsg msg { 2, 4 };

    DomainEvent& event = DomainEvent::by(msg);

    ASSERT_THAT(event.isValid(), be_true());
    ASSERT_THAT(event.transNum, eq(4));
  }
};

