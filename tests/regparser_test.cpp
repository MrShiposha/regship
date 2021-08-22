#include "catch2/catch_test_macros.hpp"

#include "regparser.hpp"

TEST_CASE("RegParser empty string", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse(""));
}

TEST_CASE("RegParser one symbol", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("a"));
}

TEST_CASE("RegParser two symbols", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("ab"));
}

TEST_CASE("RegParser or op", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("a|b"));
}

TEST_CASE("RegParser star op", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("a*"));
}

TEST_CASE("RegParser nested expr", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("(a)"));
}

TEST_CASE("RegParser nested expr star", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("(a)*"));
}

TEST_CASE("RegParser nested expr or", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("a|(b)"));
    CHECK_NOTHROW(parser.parse("(a)|b"));
    CHECK_NOTHROW(parser.parse("(a)|(b)"));
}

TEST_CASE("RegParser generic expr", "[RegParser]") {
    RegParser parser;

    CHECK_NOTHROW(parser.parse("(a|b)*abb"));
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
