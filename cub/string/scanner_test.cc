#include "cub/string/scanner.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

using namespace std::literals::string_literals;

FIXTURE(ScannerTest) {
  StringView remain;
  StringView match;

  TEST("spaces string") {
    ASSERT_TRUE(Scanner("   horse0123")
        .any(space())
        .any(digit())
        .any(alpha())
        .result(&remain, &match));
    ASSERT_EQ("   horse"s, match);
    ASSERT_EQ("0123"s, remain);
  }

  TEST("empty string") {
    ASSERT_TRUE(Scanner("")
        .any(space())
        .any(digit())
        .any(alpha())
        .result(&remain, &match));
    ASSERT_EQ(""s, remain);
    ASSERT_EQ(""s, match);
  }

  TEST("slash string") {
    ASSERT_TRUE(Scanner("----")
        .any(space())
        .any(digit())
        .any(alpha())
        .result(&remain, &match));
    ASSERT_EQ("----"s, remain);
    ASSERT_EQ(""s, match);
  }

  TEST("AnySpace") {
    ASSERT_TRUE(Scanner("  a b ")
        .any(space())
        .one(alpha())
        .any(space())
        .result(&remain, &match));
    ASSERT_EQ("  a "s, match);
    ASSERT_EQ("b "s, remain);
  }

  TEST("AnyEscapedNewline") {
    ASSERT_TRUE(Scanner("\\\n")
        .any(is_or(alpha(), digit(), underscore()))
        .result(&remain, &match));
    ASSERT_EQ("\\\n"s, remain);
    ASSERT_EQ(""s, match);
  }

  TEST("AnyEmptyString") {
    ASSERT_TRUE(Scanner("")
        .any(is_or(alpha(), digit(), underscore()))
        .result(&remain, &match));
    ASSERT_EQ(""s, remain);
    ASSERT_EQ(""s, match);
  }

  TEST("Eos") {
    ASSERT_FALSE(Scanner("a").eos().result());
    ASSERT_TRUE(Scanner("").eos().result());
    ASSERT_FALSE(Scanner("abc").literal("ab").eos().result());
    ASSERT_TRUE(Scanner("abc").literal("abc").eos().result());
  }

  TEST("Many") {
    ASSERT_TRUE(Scanner("abc").many(alpha()).result());
    ASSERT_FALSE(Scanner("0").many(alpha()).result());
    ASSERT_FALSE(Scanner("").many(alpha()).result());

    ASSERT_TRUE(
        Scanner("abc ").many(alpha()).result(&remain, &match));
    ASSERT_EQ(" "s, remain);
    ASSERT_EQ("abc"s, match);
//    ASSERT_TRUE(
//        Scanner("abc"s).many(alpha()).result(&remain, &match));
//    ASSERT_EQ(""s, remain);
//    ASSERT_EQ("abc"s, match);
  }

  TEST("one") {
    ASSERT_TRUE(Scanner("abc").one(alpha()).result());
    ASSERT_FALSE(Scanner("0").one(alpha()).result());
    ASSERT_FALSE(Scanner("").one(alpha()).result());

    ASSERT_TRUE(Scanner("abc")
        .one(alpha())
        .one(alpha())
        .result(&remain, &match));
    ASSERT_EQ("c"s, remain);
    ASSERT_EQ("ab"s, match);
    ASSERT_TRUE(Scanner("a").one(alpha()).result(&remain, &match));
    ASSERT_EQ(""s, remain);
    ASSERT_EQ("a"s, match);
  }

  TEST("OneLiteral") {
    ASSERT_FALSE(Scanner("abc").literal("abC").result());
    ASSERT_TRUE(Scanner("abc").literal("ab").literal("c").result());
  }

  TEST("ScanUntil") {
    ASSERT_TRUE(Scanner(R"(' \1 \2 \3 \' \\'rest)")
        .literal("'")
        .until(ch('\''))
        .literal("'")
        .result(&remain, &match));
    ASSERT_EQ(R"( \\'rest)"s, remain);
    ASSERT_EQ(R"(' \1 \2 \3 \')"s, match);

    // The "scan until" character is not present.
    remain = match = "unset";
    ASSERT_FALSE(Scanner(R"(' \1 \2 \3 \\rest)")
        .literal("'")
        .until(ch('\''))
        .result(&remain, &match));
    ASSERT_EQ("unset"s, remain);
    ASSERT_EQ("unset"s, match);

    // Scan until an escape character.
    remain = match = "";
    ASSERT_TRUE(Scanner(R"(123\456)")
        .until(ch('\\'))
        .result(&remain, &match));
    ASSERT_EQ(R"(\456)"s, remain);
    ASSERT_EQ("123"s, match);
  }

  TEST("ZeroOrOneLiteral") {

    ASSERT_TRUE(
        Scanner("abc").optional("abC").result(&remain, &match));
    ASSERT_EQ("abc"s, remain);
    ASSERT_EQ(""s, match);

    ASSERT_TRUE(
        Scanner("abcd").optional("ab").optional("c").result(
            &remain, &match));
    ASSERT_EQ("d"s, remain);
    ASSERT_EQ("abc"s, match);

    ASSERT_TRUE(
        Scanner("").optional("abc").result(&remain, &match));
    ASSERT_EQ(""s, remain);
    ASSERT_EQ(""s, match);
  }

  // Test output of GetResult (including the forms with optional params),
  // and that it can be called multiple times.
  TEST("CaptureAndGetResult") {


    Scanner scan("  first    second");
    ASSERT_TRUE(scan.any(space())
        .restartCapture()
        .one(alpha())
        .any(is_or(alpha(), digit()))
        .stopCapture()
        .any(space())
        .result(&remain, &match));
    ASSERT_EQ("second"s, remain);
    ASSERT_EQ("first"s, match);
    ASSERT_TRUE(scan.result());
    remain = "";
    ASSERT_TRUE(scan.result(&remain));
    ASSERT_EQ("second"s, remain);
    remain = "";
    match = "";
    ASSERT_TRUE(scan.result(&remain, &match));
    ASSERT_EQ("second"s, remain);
    ASSERT_EQ("first"s, match);

    scan.restartCapture().one(alpha()).one(alpha());
    remain = "";
    match = "";
    ASSERT_TRUE(scan.result(&remain, &match));
    ASSERT_EQ("cond"s, remain);
    ASSERT_EQ("se"s, match);
  }

  // Tests that if stopCapture is not called, then calling GetResult, then
  // scanning more, then GetResult again will update the capture.
  TEST("MultipleGetResultExtendsCapture") {


    Scanner scan("one2three");
    ASSERT_TRUE(scan.many(alpha()).result(&remain, &match));
    ASSERT_EQ("2three"s, remain);
    ASSERT_EQ("one"s, match);
    ASSERT_TRUE(scan.many(digit()).result(&remain, &match));
    ASSERT_EQ("three"s, remain);
    ASSERT_EQ("one2"s, match);
    ASSERT_TRUE(scan.many(alpha()).result(&remain, &match));
    ASSERT_EQ(""s, remain);
    ASSERT_EQ("one2three"s, match);
  }

  TEST("FailedMatchDoesntChangeResult") {
    // A failed match doesn't change pointers passed to GetResult.
    Scanner scan("name");
    StringView remaining = "rem";
    StringView match = "match";
    ASSERT_FALSE(scan.one(space()).result(&remaining, &match));
    ASSERT_EQ("rem"s, remaining);
    ASSERT_EQ("match"s, match);
  }

  TEST("DefaultCapturesAll") {
    // If RestartCapture() is not called, the whole std::string is used.
    Scanner scan("a b");
    StringView remaining = "rem";
    StringView match = "match";
    ASSERT_TRUE(scan.any(alpha())
        .any(space())
        .any(alpha())
        .result(&remaining, &match));
    ASSERT_EQ(""s, remaining);
    ASSERT_EQ("a b"s, match);
  }

  TEST("AllCharClasses") {
    ASSERT_EQ(256, matches(always()).size());
    ASSERT_EQ("0123456789"s, matches(digit()));
    ASSERT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"s,
        matches(alpha()));
    ASSERT_EQ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"s,
        matches(alnum()));

    ASSERT_EQ(
        "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
        "abcdefghijklmnopqrstuvwxyz"s,
        matches(is_or(alnum(), dash(), underscore())));

    ASSERT_EQ(
        "-./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"s,
        matches(is_or(alnum(), dash(), dot(), slash())));

    ASSERT_EQ(
        "-./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
        "abcdefghijklmnopqrstuvwxyz"s,
        matches(is_or(alnum(), dash(), dot(), slash(), underscore())));

    ASSERT_EQ(".0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(alnum(), dot())));

    ASSERT_EQ("+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(alnum(), dot(), plus(), minus())));

    ASSERT_EQ(".0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(alnum(), dot(), underscore())));

    ASSERT_EQ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(alnum(), underscore())));

    ASSERT_EQ("abcdefghijklmnopqrstuvwxyz"s, matches(lower()));

    ASSERT_EQ("0123456789abcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(lower(), digit())));

    ASSERT_EQ("0123456789_abcdefghijklmnopqrstuvwxyz"s,
              matches(is_or(lower(), digit(), underscore())));

    ASSERT_EQ("123456789"s, matches(is_and(digit(), is_not(ch('0')))));
    ASSERT_EQ("\t\n\v\f\r "s, matches(space()));
    ASSERT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ"s, matches(upper()));
  }

  TEST("Peek") {
    ASSERT_EQ('a', Scanner("abc").peek());
    ASSERT_EQ('a', Scanner("abc").peek('b'));
    ASSERT_EQ('\0', Scanner("").peek());
    ASSERT_EQ('z', Scanner("").peek('z'));
    ASSERT_EQ('A', Scanner("0123A").any(digit()).peek());
    ASSERT_EQ('\0', Scanner("0123A").any(is_or(alpha(), digit())).peek());
  }

  std::string matches(CharSpec spec) {
    std::string s;
    for (int i = 0; i < 256; ++i) {
      char ch = i;
      if (spec(ch)) {
        s += ch;
      }
    }
    return s;
  }
};

}
