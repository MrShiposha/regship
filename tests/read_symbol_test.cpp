#include "catch2/catch_test_macros.hpp"

#include "util.hpp"

TEST_CASE("read_reg_symbol Read from empty string", "[read_reg_symbol]") {
    auto sym = read_reg_symbol("");

    CHECK(sym.empty());
}

TEST_CASE("read_reg_symbol Read ASCII sym", "[read_reg_symbol]") {
    auto sym = read_reg_symbol("foo");

    CHECK((sym == "f"));
}

TEST_CASE("read_reg_symbol Read Russian sym", "[read_reg_symbol]") {
    auto sym = read_reg_symbol("фу");

    CHECK((sym == "ф"));
}

TEST_CASE("read_reg_symbol Read German sym", "[read_reg_symbol]") {
    auto sym = read_reg_symbol("über");

    CHECK((sym == "ü"));
}
