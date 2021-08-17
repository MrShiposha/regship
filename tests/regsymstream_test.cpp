#include "catch2/catch_test_macros.hpp"

#include "regsymstream.hpp"

TEST_CASE("RegSymStream empty stream", "[RegSymStream]") {
    RegSymbol sym;
    RegSymStream stream("");

    CHECK((stream) == false);
    CHECK(!stream);
    CHECK(stream.is_empty());

    stream >> sym;

    CHECK(sym.empty());
}

TEST_CASE("RegSymStream one char ASCII stream", "[RegSymStream]") {
    RegSymbol _1, _2;
    RegSymStream stream("z");

    CHECK((stream));
    CHECK(!stream == false);
    CHECK(!stream.is_empty());

    stream >> _1 >> _2;

    CHECK((_1 == "z"));
    CHECK((_2 == ""));

    CHECK((stream) == false);
    CHECK(!stream);
    CHECK(stream.is_empty());
}

TEST_CASE("RegSymStream one char Russian stream", "[RegSymStream]") {
    RegSymbol _1, _2;
    RegSymStream stream("ч");

    CHECK((stream));
    CHECK(!stream == false);
    CHECK(!stream.is_empty());

    stream >> _1 >> _2;

    CHECK((_1 == "ч"));
    CHECK((_2 == ""));

    CHECK((stream) == false);
    CHECK(!stream);
    CHECK(stream.is_empty());
}

TEST_CASE("RegSymStream one char German stream", "[RegSymStream]") {
    RegSymbol _1, _2;
    RegSymStream stream("ß");

    CHECK((stream));
    CHECK(!stream == false);
    CHECK(!stream.is_empty());

    stream >> _1 >> _2;

    CHECK((_1 == "ß"));
    CHECK((_2 == ""));

    CHECK((stream) == false);
    CHECK(!stream);
    CHECK(stream.is_empty());
}

TEST_CASE("RegSymStream mixed stream", "[RegSymStream]") {
    RegSymbol _1, _2, _3, _4;
    RegSymStream stream("Qжö");

    CHECK((stream));
    CHECK(!stream == false);
    CHECK(!stream.is_empty());

    stream >> _1 >> _2 >> _3 >> _4;

    CHECK((_1 == "Q"));
    CHECK((_2 == "ж"));
    CHECK((_3 == "ö"));
    CHECK((_4 == ""));

    CHECK((stream) == false);
    CHECK(!stream);
    CHECK(stream.is_empty());
}
