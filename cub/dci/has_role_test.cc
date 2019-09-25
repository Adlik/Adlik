#include "cub/dci/role.h"
#include "cut/cut.hpp"

using namespace cum;
using namespace cub;

namespace {
  DEFINE_ROLE(Energy) {
    ABSTRACT(void consume());
    ABSTRACT(bool isExhausted() const);
  };

  struct HumanEnergy: Energy {
    HumanEnergy()
        : isHungry(false), consumeTimes(0) {
    }

  private:
    OVERRIDE(void consume()) {
      consumeTimes++;

      if (consumeTimes >= MAX_CONSUME_TIMES) {
        isHungry = true;
      }
    }

    OVERRIDE(bool isExhausted() const) {
      return isHungry;
    }

  private:
    enum {
      MAX_CONSUME_TIMES = 10
    };

    bool isHungry;
    uint8_t consumeTimes;
  };

  DEFINE_ROLE(Worker) {
    Worker()
        : produceNum(0) {
    }

    void produce() {
      if (ROLE(Energy).isExhausted())
        return;

      produceNum++;

      ROLE(Energy).consume();
    }

    uint32_t getProduceNum() const {
      return produceNum;
    }

  private:
    uint32_t produceNum;

  private:
    USE_ROLE(Energy);
  };

  DEFINE_ROLE(Human) {
    HAS_ROLE(Worker);
  };

  struct HumanObject: Human, private Worker, private HumanEnergy {
  private:
    IMPL_ROLE(Worker)
    ;IMPL_ROLE(Energy)
    ;
  };

  struct HumanFactory {
    static Human* create() {
      return new HumanObject;
    }
  };
}

FIXTURE(HasRoleTest) {
  TEST("should visit all the role correctly through has role interface") {
    Human* human = HumanFactory::create();
    human->ROLE(Worker).produce();

    ASSERT_THAT(human->ROLE(Worker).getProduceNum(), eq(1));
    delete human;
  }
};
