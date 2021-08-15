#include "catch2/catch_test_macros.hpp"

#include "regdfa.hpp"

TEST_CASE("Test empty", "[RegDFA]") {
    RegPosSets empty("");
    RegDFA dfa(empty);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    CHECK(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    CHECK(dfa.current_state() == first_state);
}

TEST_CASE("Test one symbol", "[RegDFA]") {
    RegPosSets a_pos_sets("a");
    RegDFA dfa(a_pos_sets);

    auto first_state = dfa.current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    CHECK(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("a") == RegDFA::DEADSTATE);
    CHECK(dfa.current_state() == first_state);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    CHECK(dfa.current_state() == first_state);
}

TEST_CASE("Test concat", "[RegDFA]") {
    {
        auto concat = RegPosSets("a").concat("b");
        RegDFA dfa(concat);

        auto first_state = dfa.current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        CHECK(dfa.current_state() != first_state);

        auto second_state = dfa.current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        CHECK(dfa.current_state() == second_state);
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
        CHECK(dfa.current_state() == third_state);
    }
}
