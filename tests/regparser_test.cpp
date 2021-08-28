#include "catch2/catch_test_macros.hpp"

#include "regparser.hpp"

TEST_CASE("RegParser empty string", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse(""));

    auto matcher = parser.parse("");
    CHECK(matcher.is_matches(""));
}

TEST_CASE("RegParser one symbol", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("a"));

    auto matcher = parser.parse("a");
    CHECK(matcher.is_matches("a"));
}

TEST_CASE("RegParser two symbols", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("ab"));

    auto matcher = parser.parse("ab");
    CHECK(matcher.is_matches("ab"));
}

TEST_CASE("RegParser or op", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("a|b"));

    auto matcher = parser.parse("a|b");
    CHECK(matcher.is_matches("a"));
    CHECK(matcher.is_matches("b"));
}

TEST_CASE("RegParser star op", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("a*"));
    auto matcher = parser.parse("a*");
    CHECK(matcher.is_matches(""));
    CHECK(matcher.is_matches("a"));
    CHECK(matcher.is_matches("aa"));
    CHECK(matcher.is_matches("aaa"));
}

TEST_CASE("RegParser nested expr", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("(a)"));
    auto matcher = parser.parse("(a)");
    CHECK(matcher.is_matches("a"));
}

TEST_CASE("RegParser nested expr star", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("(a)*"));
    auto matcher = parser.parse("(a)*");
    CHECK(matcher.is_matches(""));
    CHECK(matcher.is_matches("a"));
    CHECK(matcher.is_matches("aa"));
    CHECK(matcher.is_matches("aaa"));
}

TEST_CASE("RegParser nested expr or", "[RegParser]") {
    RegParser parser;

    {
        REQUIRE_NOTHROW(parser.parse("a|(b)"));
        auto matcher = parser.parse("a|(b)");
        CHECK(matcher.is_matches("a"));
        CHECK(matcher.is_matches("b"));
    }

    {
        REQUIRE_NOTHROW(parser.parse("(a)|b"));
        auto matcher = parser.parse("(a)|b");
        CHECK(matcher.is_matches("a"));
        CHECK(matcher.is_matches("b"));
    }

    {
        REQUIRE_NOTHROW(parser.parse("(a)|(b)"));
        auto matcher = parser.parse("(a)|(b)");
        CHECK(matcher.is_matches("a"));
        CHECK(matcher.is_matches("b"));
    }
}

TEST_CASE("RegParser generic expr", "[RegParser]") {
    RegParser parser;

    REQUIRE_NOTHROW(parser.parse("(a|b)*abb"));
    auto matcher = parser.parse("(a|b)*abb");
    CHECK(matcher.is_matches("abb"));
    CHECK(matcher.is_matches("babb"));
    CHECK(matcher.is_matches("aabb"));
    CHECK(matcher.is_matches("ababb"));
    CHECK(matcher.is_matches("abbabb"));
    CHECK(matcher.is_matches("abbaabb"));
    CHECK(!matcher.is_matches("abbaabba"));
    CHECK(!matcher.is_matches(""));
}

TEST_CASE("RegParser invalid expr", "[RegParser]") {
    RegParser parser;

    CHECK_THROWS(parser.parse("("));
    CHECK_THROWS(parser.parse(")"));
    CHECK_THROWS(parser.parse("()"));
    CHECK_THROWS(parser.parse("*"));
    CHECK_THROWS(parser.parse("|"));
    CHECK_THROWS(parser.parse("a**"));
    CHECK_THROWS(parser.parse("a*|"));
    CHECK_THROWS(parser.parse("a|*"));
    CHECK_THROWS(parser.parse("a)"));
    CHECK_THROWS(parser.parse("a))"));
    CHECK_THROWS(parser.parse("(a"));
    CHECK_THROWS(parser.parse("((a"));
}
