#include "cub/base/optional.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

namespace {
  struct StructorListener {
    int construct0 = 0;
    int construct1 = 0;
    int construct2 = 0;
    int listinit = 0;
    int copy = 0;
    int move = 0;
    int copy_assign = 0;
    int move_assign = 0;
    int destruct = 0;
  };

  struct Listenable {
    static StructorListener* listener;

    Listenable() {
      ++listener->construct0;
    }

    Listenable(int) {
      ++listener->construct1;
    }

    Listenable(int, int) {
      ++listener->construct2;
    }

    Listenable(std::initializer_list<int>) {
      ++listener->listinit;
    }

    Listenable(const Listenable&) {
      ++listener->copy;
    }

    Listenable(Listenable&&) noexcept {
      ++listener->move;
    }

    Listenable& operator=(const Listenable&) {
      ++listener->copy_assign;
      return *this;
    }

    Listenable& operator=(Listenable&&) noexcept {
      ++listener->move_assign;
      return *this;
    }

    ~Listenable() {
      ++listener->destruct;
    }
  };

  StructorListener* Listenable::listener = nullptr;
}

FIXTURE(OptionalTest) {
  TEST("DefaultConstructor") {
    Optional<int> empty;
    ASSERT_FALSE(empty);
  }

  TEST("NullOptConstructor") {
    Optional<int> empty(nilopt);
    ASSERT_FALSE(empty);
  }

  TEST("CopyConstructor") {
    Optional<int> empty, opt42 = 42;
    Optional<int> empty_copy(empty);
    ASSERT_FALSE(empty_copy);
    Optional<int> opt42_copy(opt42);
    ASSERT_TRUE(opt42_copy);
    ASSERT_EQ(42, *opt42_copy);
  }

  TEST("StructorBasic") {
    StructorListener listener;
    Listenable::listener = &listener;
    {
      Optional<Listenable> empty;
      ASSERT_FALSE(empty);
      Optional<Listenable> opt0(inplace);
      ASSERT_TRUE(opt0);
      Optional<Listenable> opt1(inplace, 1);
      ASSERT_TRUE(opt1);
      Optional<Listenable> opt2(inplace, 1, 2);
      ASSERT_TRUE(opt2);
    }
    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(1, listener.construct1);
    ASSERT_EQ(1, listener.construct2);
    ASSERT_EQ(3, listener.destruct);
  }

  TEST("CopyMoveStructor") {
    StructorListener listener;
    Listenable::listener = &listener;
    Optional<Listenable> original(inplace);
    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(0, listener.copy);
    ASSERT_EQ(0, listener.move);
    Optional<Listenable> copy(original);
    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(1, listener.copy);
    ASSERT_EQ(0, listener.move);
    Optional<Listenable> move(std::move(original));
    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(1, listener.copy);
    ASSERT_EQ(1, listener.move);
  }

  TEST("ListInit") {
    StructorListener listener;
    Listenable::listener = &listener;
    Optional<Listenable> listinit1(inplace, { 1 });
    Optional<Listenable> listinit2(inplace, { 1, 2 });
    ASSERT_EQ(2, listener.listinit);
  }

  TEST("CopyAssignment") {
    const Optional<int> empty, opt1 = 1, opt2 = 2;
    Optional<int> empty_to_opt1, opt1_to_opt2, opt2_to_empty;

    ASSERT_FALSE(empty_to_opt1);
    empty_to_opt1 = empty;
    ASSERT_FALSE(empty_to_opt1);
    empty_to_opt1 = opt1;
    ASSERT_TRUE(empty_to_opt1);
    ASSERT_EQ(1, empty_to_opt1.value());

    ASSERT_FALSE(opt1_to_opt2);
    opt1_to_opt2 = opt1;
    ASSERT_TRUE(opt1_to_opt2);
    ASSERT_EQ(1, opt1_to_opt2.value());
    opt1_to_opt2 = opt2;
    ASSERT_TRUE(opt1_to_opt2);
    ASSERT_EQ(2, opt1_to_opt2.value());

    ASSERT_FALSE(opt2_to_empty);
    opt2_to_empty = opt2;
    ASSERT_TRUE(opt2_to_empty);
    ASSERT_EQ(2, opt2_to_empty.value());
    opt2_to_empty = empty;
    ASSERT_FALSE(opt2_to_empty);
  }

  TEST("MoveAssignment") {
    StructorListener listener;
    Listenable::listener = &listener;

    Optional<Listenable> empty1, empty2, set1(inplace), set2(inplace);
    ASSERT_EQ(2, listener.construct0);
    Optional<Listenable> empty_to_empty, empty_to_set, set_to_empty(inplace),
        set_to_set(inplace);
    ASSERT_EQ(4, listener.construct0);
    empty_to_empty = std::move(empty1);  // donothing
    empty_to_set = std::move(set1);      // move constructor,
    set_to_empty = std::move(empty2);    // destructor
    set_to_set = std::move(set2);        // move assignment
    ASSERT_EQ(0, listener.copy);
    ASSERT_EQ(1, listener.move);
    ASSERT_EQ(1, listener.destruct);
    ASSERT_EQ(1, listener.move_assign);
  }

  TEST("AssignmentVarious") {
    Optional<int> opt;
    ASSERT_FALSE(opt);
    opt = 42;
    ASSERT_TRUE(opt);
    ASSERT_EQ(42, opt.value());
    opt = nilopt;
    ASSERT_FALSE(opt);
    opt = 42;
    ASSERT_TRUE(opt);
    ASSERT_EQ(42, opt.value());
    opt = 43;
    ASSERT_TRUE(opt);
    ASSERT_EQ(43, opt.value());
  }

  TEST("Emplace") {
    StructorListener listener;
    Listenable::listener = &listener;
    Optional<Listenable> opt;
    ASSERT_FALSE(opt);
    opt.emplace(1);
    ASSERT_TRUE(opt);
    opt.emplace(1, 2);
    ASSERT_EQ(1, listener.construct1);
    ASSERT_EQ(1, listener.construct2);
    ASSERT_EQ(1, listener.destruct);
  }

  TEST("Swap") {
    Optional<int> opt_empty, opt1 = 1, opt2 = 2;
    ASSERT_FALSE(opt_empty);
    ASSERT_TRUE(opt1);
    ASSERT_EQ(1, opt1.value());
    ASSERT_TRUE(opt2);
    ASSERT_EQ(2, opt2.value());
    swap(opt_empty, opt1);
    ASSERT_FALSE(opt1);
    ASSERT_TRUE(opt_empty);
    ASSERT_EQ(1, opt_empty.value());
    ASSERT_TRUE(opt2);
    ASSERT_EQ(2, opt2.value());
    swap(opt_empty, opt1);
    ASSERT_FALSE(opt_empty);
    ASSERT_TRUE(opt1);
    ASSERT_EQ(1, opt1.value());
    ASSERT_TRUE(opt2);
    ASSERT_EQ(2, opt2.value());
    swap(opt1, opt2);
    ASSERT_FALSE(opt_empty);
    ASSERT_TRUE(opt1);
    ASSERT_EQ(2, opt1.value());
    ASSERT_TRUE(opt2);
    ASSERT_EQ(1, opt2.value());
  }

  TEST("PointerStuff") {
    Optional<std::string> opt(inplace, "foo");
    ASSERT_EQ(std::string("foo"), *opt);
    const auto& opt_const = opt;
    ASSERT_EQ(std::string("foo"), *opt_const);
    ASSERT_EQ(opt->size(), 3);
    ASSERT_EQ(opt_const->size(), 3);
  }

  TEST("Value") {
    using O = Optional<std::string>;
    using CO = const Optional<std::string>;
    O lvalue(inplace, "lvalue");
    CO clvalue(inplace, "clvalue");
    ASSERT_EQ(std::string("lvalue"), lvalue.value());
    ASSERT_EQ(std::string("clvalue"), clvalue.value());
    ASSERT_EQ(std::string("xvalue"), O(inplace, "xvalue").value());
    ASSERT_EQ(std::string("cxvalue"), CO(inplace, "cxvalue").value());
  }

  TEST("ValueOr") {
    Optional<double> opt_empty, opt_set = 1.2;
    ASSERT_EQ(42.0, opt_empty.value_or(42));
    ASSERT_EQ(1.2, opt_set.value_or(42));
    ASSERT_EQ(42.0, Optional<double>().value_or(42));
    ASSERT_EQ(1.2, Optional<double>(1.2).value_or(42));
  }

  TEST("make_optional") {
    ASSERT_EQ(42, make_optional(42).value());
  }

  TEST("Comparisons") {
    Optional<int> ae, be, a2 = 2, b2 = 2, a4 = 4, b4 = 4;

  #define OptionalTest_Comparisons_EXPECT_LESS(x, y) \
    ASSERT_FALSE((x) == (y));                        \
    ASSERT_TRUE((x) != (y));                         \
    ASSERT_TRUE((x) < (y));                          \
    ASSERT_FALSE((x) > (y));                         \
    ASSERT_TRUE((x) <= (y));                         \
    ASSERT_FALSE((x) >= (y));

  #define OptionalTest_Comparisons_EXPECT_SAME(x, y) \
    ASSERT_TRUE((x) == (y));                         \
    ASSERT_FALSE((x) != (y));                        \
    ASSERT_FALSE((x) < (y));                         \
    ASSERT_FALSE((x) > (y));                         \
    ASSERT_TRUE((x) <= (y));                         \
    ASSERT_TRUE((x) >= (y));

  #define OptionalTest_Comparisons_EXPECT_GREATER(x, y) \
    ASSERT_FALSE((x) == (y));                           \
    ASSERT_TRUE((x) != (y));                            \
    ASSERT_FALSE((x) < (y));                            \
    ASSERT_TRUE((x) > (y));                             \
    ASSERT_FALSE((x) <= (y));                           \
    ASSERT_TRUE((x) >= (y));

    // LHS: nullopt, ae, a2, 3, a4
    // RHS: nullopt, be, b2, 3, b4

    // OptionalTest_Comparisons_EXPECT_NOT_TO_WORK(nullopt,nullopt);
    OptionalTest_Comparisons_EXPECT_SAME(nilopt, be);
    OptionalTest_Comparisons_EXPECT_LESS(nilopt, b2);
    // OptionalTest_Comparisons_EXPECT_NOT_TO_WORK(nullopt,3);
    OptionalTest_Comparisons_EXPECT_LESS(nilopt, b4);

    OptionalTest_Comparisons_EXPECT_SAME(ae, nilopt);
    OptionalTest_Comparisons_EXPECT_SAME(ae, be);
    OptionalTest_Comparisons_EXPECT_LESS(ae, b2);
    OptionalTest_Comparisons_EXPECT_LESS(ae, 3);
    OptionalTest_Comparisons_EXPECT_LESS(ae, b4);

    OptionalTest_Comparisons_EXPECT_GREATER(a2, nilopt);
    OptionalTest_Comparisons_EXPECT_GREATER(a2, be);
    OptionalTest_Comparisons_EXPECT_SAME(a2, b2);
    OptionalTest_Comparisons_EXPECT_LESS(a2, 3);
    OptionalTest_Comparisons_EXPECT_LESS(a2, b4);

    // OptionalTest_Comparisons_EXPECT_NOT_TO_WORK(3,nullopt);
    OptionalTest_Comparisons_EXPECT_GREATER(3, be);
    OptionalTest_Comparisons_EXPECT_GREATER(3, b2);
    OptionalTest_Comparisons_EXPECT_SAME(3, 3);
    OptionalTest_Comparisons_EXPECT_LESS(3, b4);

    OptionalTest_Comparisons_EXPECT_GREATER(a4, nilopt);
    OptionalTest_Comparisons_EXPECT_GREATER(a4, be);
    OptionalTest_Comparisons_EXPECT_GREATER(a4, b2);
    OptionalTest_Comparisons_EXPECT_GREATER(a4, 3);
    OptionalTest_Comparisons_EXPECT_SAME(a4, b4);
  }

  TEST("SwapRegression") {
    StructorListener listener;
    Listenable::listener = &listener;

    {
      Optional<Listenable> a;
      Optional<Listenable> b(inplace);
      a.swap(b);
    }

    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(1, listener.move);
    ASSERT_EQ(2, listener.destruct);

    {
      Optional<Listenable> a(inplace);
      Optional<Listenable> b;
      a.swap(b);
    }

    ASSERT_EQ(2, listener.construct0);
    ASSERT_EQ(2, listener.move);
    ASSERT_EQ(4, listener.destruct);
  }

  TEST("BigstringLeakCheck") {
    constexpr size_t n = 1 << 16;

    using OS = Optional<std::string>;

    OS a;
    OS b = nilopt;
    OS c = std::string(n, 'c');
    std::string sd(n, 'd');
    OS d = sd;
    OS e(inplace, n, 'e');
    OS f;
    f.emplace(n, 'f');

    OS ca(a);
    OS cb(b);
    OS cc(c);
    OS cd(d);
    OS ce(e);

    OS oa;
    OS ob = nilopt;
    OS oc = std::string(n, 'c');
    std::string sod(n, 'd');
    OS od = sod;
    OS oe(inplace, n, 'e');
    OS of;
    of.emplace(n, 'f');

    OS ma(std::move(oa));
    OS mb(std::move(ob));
    OS mc(std::move(oc));
    OS md(std::move(od));
    OS me(std::move(oe));
    OS mf(std::move(of));

    OS aa1;
    OS ab1 = nilopt;
    OS ac1 = std::string(n, 'c');
    std::string sad1(n, 'd');
    OS ad1 = sad1;
    OS ae1(inplace, n, 'e');
    OS af1;
    af1.emplace(n, 'f');

    OS aa2;
    OS ab2 = nilopt;
    OS ac2 = std::string(n, 'c');
    std::string sad2(n, 'd');
    OS ad2 = sad2;
    OS ae2(inplace, n, 'e');
    OS af2;
    af2.emplace(n, 'f');

    aa1 = af2;
    ab1 = ae2;
    ac1 = ad2;
    ad1 = ac2;
    ae1 = ab2;
    af1 = aa2;

    OS aa3;
    OS ab3 = nilopt;
    OS ac3 = std::string(n, 'c');
    std::string sad3(n, 'd');
    OS ad3 = sad3;
    OS ae3(inplace, n, 'e');
    OS af3;
    af3.emplace(n, 'f');

    aa3 = nilopt;
    ab3 = nilopt;
    ac3 = nilopt;
    ad3 = nilopt;
    ae3 = nilopt;
    af3 = nilopt;

    OS aa4;
    OS ab4 = nilopt;
    OS ac4 = std::string(n, 'c');
    std::string sad4(n, 'd');
    OS ad4 = sad4;
    OS ae4(inplace, n, 'e');
    OS af4;
    af4.emplace(n, 'f');

    aa4 = OS(inplace, n, 'a');
    ab4 = OS(inplace, n, 'b');
    ac4 = OS(inplace, n, 'c');
    ad4 = OS(inplace, n, 'd');
    ae4 = OS(inplace, n, 'e');
    af4 = OS(inplace, n, 'f');

    OS aa5;
    OS ab5 = nilopt;
    OS ac5 = std::string(n, 'c');
    std::string sad5(n, 'd');
    OS ad5 = sad5;
    OS ae5(inplace, n, 'e');
    OS af5;
    af5.emplace(n, 'f');

    std::string saa5(n, 'a');
    std::string sab5(n, 'a');
    std::string sac5(n, 'a');
    std::string sad52(n, 'a');
    std::string sae5(n, 'a');
    std::string saf5(n, 'a');

    aa5 = saa5;
    ab5 = sab5;
    ac5 = sac5;
    ad5 = sad52;
    ae5 = sae5;
    af5 = saf5;

    OS aa6;
    OS ab6 = nilopt;
    OS ac6 = std::string(n, 'c');
    std::string sad6(n, 'd');
    OS ad6 = sad6;
    OS ae6(inplace, n, 'e');
    OS af6;
    af6.emplace(n, 'f');

    aa6 = std::string(n, 'a');
    ab6 = std::string(n, 'b');
    ac6 = std::string(n, 'c');
    ad6 = std::string(n, 'd');
    ae6 = std::string(n, 'e');
    af6 = std::string(n, 'f');

    OS aa7;
    OS ab7 = nilopt;
    OS ac7 = std::string(n, 'c');
    std::string sad7(n, 'd');
    OS ad7 = sad7;
    OS ae7(inplace, n, 'e');
    OS af7;
    af7.emplace(n, 'f');

    aa7.emplace(n, 'A');
    ab7.emplace(n, 'B');
    ac7.emplace(n, 'C');
    ad7.emplace(n, 'D');
    ae7.emplace(n, 'E');
    af7.emplace(n, 'F');
  }

  TEST("MoveAssignRegression") {
    StructorListener listener;
    Listenable::listener = &listener;

    {
      Optional<Listenable> a;
      Listenable b;
      a = std::move(b);
    }

    ASSERT_EQ(1, listener.construct0);
    ASSERT_EQ(1, listener.move);
    ASSERT_EQ(2, listener.destruct);
  }

};

}
