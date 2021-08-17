#include "catch2/catch_test_macros.hpp"

#include "util.hpp"

TEST_CASE("read_symbol Read from empty string", "[read_symbol]") {
    auto sym = read_symbol("");

    CHECK(sym.empty());
}

TEST_CASE("read_symbol Read ASCII sym", "[read_symbol]") {
    auto sym = read_symbol("foo");

    CHECK((sym == "f"));
}

TEST_CASE("read_symbol Read Russian sym", "[read_symbol]") {
    auto sym = read_symbol("фу");

    CHECK((sym == "ф"));
}

TEST_CASE("read_symbol Read German sym", "[read_symbol]") {
    auto sym = read_symbol("über");

    CHECK((sym == "ü"));
}
