#include "cub/base/placement.h"
#include "cub/algo/loop.h"
#include "cub/dci/role.h"
#include "cut/cut.hpp"

using namespace cum;
using namespace cub;

namespace {
  struct Student {
    Student(uint32_t id)
        : id(id) {
    }

    uint32_t getId() const {
      return id;
    }

  private:
    uint32_t id;
  };
}

FIXTURE(PlacementTest1) {
  TEST("should new object on placement memory") {
    Placement<Student> studentMemory;
    auto student = new (studentMemory.alloc()) Student(5);

    ASSERT_THAT(student->getId(), eq(5));
    ASSERT_THAT(studentMemory.getRef().getId(), eq(5));
    ASSERT_THAT(studentMemory->getId(), eq(5));
    ASSERT_THAT((*studentMemory).getId(), eq(5));

    studentMemory.destroy();
  }

  TEST("should new object array on placement") {
    const uint8_t MAX_ENGINE = 5;
    Placement<Student> students[MAX_ENGINE];

    FOREACH(i, MAX_ENGINE) {
      new (students[i].alloc()) Student(i);
    }

    FOREACH(i, MAX_ENGINE) {
      ASSERT_THAT(students[i]->getId(), eq(i));
    }
  }
};

namespace {
  const unsigned int INVALID_ID = 0xFFFFFFFF;

  struct Member {
    Member(uint32_t id) : id(id) {
    }

    uint32_t getId() const {
      return id;
    }

  private:
    uint32_t id;
  };

  struct Object {
    Object() : member(nullptr) {
    }

    void updateId(uint32_t id) {
      if (member != nullptr)
        return;
      member = new (memory.alloc()) Member(id);
    }

    uint32_t getId() const {
      if (member == nullptr)
        return INVALID_ID;
      return member->getId();
    }

    ~Object() {
      if (member != nullptr)
        memory.destroy();
    }

  private:
    Member* member;
    Placement<Member> memory;
  };
}

FIXTURE(PlacementTest2) {
  TEST("should delay init the member object in placement") {
    Object object;

    ASSERT_THAT(object.getId(), eq(INVALID_ID));

    object.updateId(5);
    ASSERT_THAT(object.getId(), eq(5));
  }
};

namespace {
  DEFINE_ROLE(Energy) {
    ABSTRACT(void consume());
    ABSTRACT(bool isExhausted() const);
  };

  const int MAX_CONSUME_TIMES = 10;

  struct HumanEnergy: Energy {
    HumanEnergy() : isHungry(false), consumeTimes(0) {
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
    bool isHungry;
    uint8_t consumeTimes;
  };

  const int FULL_PERCENT = 100;
  const int CONSUME_PERCENT = 1;

  struct ChargeEnergy: Energy {
    ChargeEnergy() : percent(FULL_PERCENT) {
    }

  private:
    OVERRIDE(void consume()) {
      if (percent > 0)
        percent -= CONSUME_PERCENT;
    }

    OVERRIDE(bool isExhausted() const) {
      return percent == 0;
    }

  private:
    uint8_t percent;
  };

  DEFINE_ROLE(Worker) {
    Worker() : produceNum(0) {
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

    bool isExhausted() const {
      return ROLE(Energy).isExhausted();
    }

  private:
    uint32_t produceNum;

  private:
    USE_ROLE(Energy);
  };

  enum WorkerType {
    HUMAN, ROBOT
  };

  struct WorkerObject: Worker {
    WorkerObject(WorkerType type) : energy(create(type)) {
    }

  private:
    Energy* create(WorkerType type) {
      switch (type) {
      case HUMAN: return new (m.human.alloc()) HumanEnergy();
      case ROBOT: return new (m.robot.alloc()) ChargeEnergy();
      default: return nullptr;
      }
    }

  private:
    union {
      Placement<HumanEnergy> human;
      Placement<ChargeEnergy> robot;
    } m;

    Energy *energy;

  private:
    IMPL_ROLE_WITH_OBJ(Energy, *energy)
    ;
  };
}

FIXTURE(PlacementTest3) {
  TEST("should cast to the public role correctly for human") {
    WorkerObject human(HUMAN);

    while (!UPCAST(human, Worker).isExhausted()) {
      UPCAST(human, Worker).produce();
    }
    ASSERT_THAT(UPCAST(human, Worker).getProduceNum(), eq(MAX_CONSUME_TIMES));
  }

  TEST("should cast to the public role correctly for robot") {
    WorkerObject robot(ROBOT);

    while (!UPCAST(robot, Worker).isExhausted()) {
      UPCAST(robot, Worker).produce();
    }
    ASSERT_THAT(UPCAST(robot, Worker).getProduceNum(),
        eq(FULL_PERCENT / CONSUME_PERCENT));
  }
};
