#include "catch2/catch_test_macros.hpp"
#include "regmatcher.hpp"

TEST_CASE("RegMatcher empty string", "[RegMatcher]") {
    RegMatcher matcher("");

    CHECK(matcher.is_matches(""));
    CHECK(!matcher.is_matches("<not existing symbol>"));
}

TEST_CASE("RegMatcher one sym", "[RegMatcher]") {
    RegMatcher matcher("a");

    CHECK(matcher.is_matches("a"));
    CHECK(!matcher.is_matches("aa"));
    CHECK(!matcher.is_matches(""));
    CHECK(!matcher.is_matches("<not existing symbol>"));
}

TEST_CASE("RegMatcher concat", "[RegMatcher]") {
    RegMatcher matcher(RegPosSets("a").concat("b"));

    CHECK(matcher.is_matches("ab"));
    CHECK(!matcher.is_matches("aba"));
    CHECK(!matcher.is_matches("a"));
    CHECK(!matcher.is_matches("b"));
    CHECK(!matcher.is_matches("ba"));
    CHECK(!matcher.is_matches("aa"));
    CHECK(!matcher.is_matches("bb"));
    CHECK(!matcher.is_matches(""));
    CHECK(!matcher.is_matches("<not existing symbol>"));
}

TEST_CASE("RegMatcher or", "[RegMatcher]") {
    RegMatcher matcher(RegPosSets("a").or("b"));

    CHECK(matcher.is_matches("a"));
    CHECK(matcher.is_matches("b"));
    CHECK(!matcher.is_matches("ab"));
    CHECK(!matcher.is_matches("aba"));
    CHECK(!matcher.is_matches("ba"));
    CHECK(!matcher.is_matches("aa"));
    CHECK(!matcher.is_matches("bb"));
    CHECK(!matcher.is_matches(""));
    CHECK(!matcher.is_matches("<not existing symbol>"));
}

TEST_CASE("RegMatcher star", "[RegMatcher]") {
    RegMatcher matcher(RegPosSets("a").star());

    CHECK(matcher.is_matches(""));
    CHECK(matcher.is_matches("a"));
    CHECK(matcher.is_matches("aa"));
    CHECK(matcher.is_matches("aaa"));
    CHECK(!matcher.is_matches("ab"));
    CHECK(!matcher.is_matches("aba"));
    CHECK(!matcher.is_matches("ba"));
    CHECK(!matcher.is_matches("bb"));
    CHECK(!matcher.is_matches("<not existing symbol>"));
}

TEST_CASE("RegMatcher reg expr", "[RegMatcher]") {
    auto expr = ((RegPosSets("a").or("b")).star())
                    .concat("a")
                    .concat("b")
                    .concat("b");

    RegMatcher matcher(expr);
    CHECK(matcher.is_matches("abb"));
    CHECK(matcher.is_matches("babb"));
    CHECK(matcher.is_matches("aabb"));
    CHECK(matcher.is_matches("ababb"));
    CHECK(matcher.is_matches("abbabb"));
    CHECK(matcher.is_matches("abbaabb"));
    CHECK(!matcher.is_matches("abbaabba"));
    CHECK(!matcher.is_matches(""));
}
