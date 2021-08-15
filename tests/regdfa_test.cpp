#include "catch2/catch_test_macros.hpp"

#include "regdfa.hpp"

TEST_CASE("RegDFA Test empty", "[RegDFA]") {
    RegPosSets empty("");
    RegDFA dfa(empty);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    REQUIRE(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    REQUIRE(dfa.current_state() == first_state);
}

TEST_CASE("RegDFA Test one symbol", "[RegDFA]") {
    RegPosSets a_pos_sets("a");
    RegDFA dfa(a_pos_sets);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    REQUIRE(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("a") == RegDFA::DEADSTATE);
    REQUIRE(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    REQUIRE(dfa.current_state() == first_state);
}

TEST_CASE("RegDFA Test concat", "[RegDFA]") {
    {
        auto concat = RegPosSets("a").concat("b");
        RegDFA dfa(concat);

        auto first_state = dfa.current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        REQUIRE(dfa.current_state() != first_state);

        auto second_state = dfa.current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        REQUIRE(dfa.current_state() == second_state);
    }

    {
        auto concat = RegPosSets("a").concat("b").concat("b");
        RegDFA dfa(concat);

        auto first_state = dfa.current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        REQUIRE(dfa.current_state() != first_state);

        auto second_state = dfa.current_state();

        CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
        REQUIRE(dfa.current_state() != second_state);

        auto third_state = dfa.current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        REQUIRE(dfa.current_state() == third_state);
    }
}

TEST_CASE("RegDFA Test Or Concat", "[RegDFA]") {
    auto expr = RegPosSets("a").or("b").concat("c");
    RegDFA dfa(expr);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != first_state);

    auto second_state = dfa.current_state();

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);

    dfa.set_current_state(first_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != first_state);

    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);
}

TEST_CASE("RegDFA Test Concat Or", "[RegDFA]") {
    auto expr = RegPosSets("a").concat(RegPosSets("b").or("c"));
    RegDFA dfa(expr);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != first_state);

    auto second_state = dfa.current_state();

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);

    dfa.set_current_state(first_state);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != first_state);

    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
}

TEST_CASE("RegDFA Test Star", "[RegDFA]") {
    auto expr = RegPosSets("a").star();
    RegDFA dfa(expr);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == first_state);
}

TEST_CASE("RegDFA generic expr", "[RegDFA]") {
    auto expr = (RegPosSets("a").or("b").star())
                .concat("a")
                .concat("b")
                .concat("b");
    RegDFA dfa(expr);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != first_state);

    auto second_state = dfa.current_state();

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != second_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == first_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == first_state);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != second_state);

    auto third_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == third_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() != third_state);

    auto final_state = dfa.current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == second_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == third_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.current_state() == final_state);
}
