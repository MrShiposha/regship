#include <limits>

#include "catch2/catch_test_macros.hpp"

#include "regdfa.hpp"

static const RegPosSets::SymbolPos IMPOSSIBLE_POS =
    std::numeric_limits<RegPosSets::SymbolPos>::max();

TEST_CASE("RegDFA Test empty", "[RegDFA]") {
    RegPosSets empty("");
    RegDFA dfa(empty, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
}

TEST_CASE("RegDFA Test one symbol", "[RegDFA]") {
    RegPosSets a_pos_sets("a");
    RegDFA dfa(a_pos_sets, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
    CHECK(dfa.do_transition("a") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
}

TEST_CASE("RegDFA Test concat", "[RegDFA]") {
    {
        auto concat = RegPosSets("a").concat("b");
        RegDFA dfa(concat, IMPOSSIBLE_POS);

        auto first_state = dfa.get_current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        REQUIRE(dfa.get_current_state().id != first_state.id);

        auto second_state = dfa.get_current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        REQUIRE(dfa.get_current_state().id == second_state.id);
    }

    {
        auto concat = RegPosSets("a").concat("b").concat("b");
        RegDFA dfa(concat, IMPOSSIBLE_POS);

        auto first_state = dfa.get_current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        REQUIRE(dfa.get_current_state().id != first_state.id);

        auto second_state = dfa.get_current_state();

        CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
        REQUIRE(dfa.get_current_state().id != second_state.id);

        auto third_state = dfa.get_current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        REQUIRE(dfa.get_current_state().id == third_state.id);
    }
}

TEST_CASE("RegDFA Test Or Concat", "[RegDFA]") {
    auto expr = RegPosSets("a").or("b").concat("c");
    RegDFA dfa(expr, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);

    auto second_state = dfa.get_current_state();

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);

    dfa.set_current_state(first_state);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);

    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);
}

TEST_CASE("RegDFA Test Concat Or", "[RegDFA]") {
    auto expr = RegPosSets("a").concat(RegPosSets("b").or("c"));
    RegDFA dfa(expr, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);

    auto second_state = dfa.get_current_state();

    CHECK(dfa.do_transition("c") == RegDFA::DEADSTATE);

    dfa.set_current_state(first_state);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);

    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
}

TEST_CASE("RegDFA Test Star", "[RegDFA]") {
    auto expr = RegPosSets("a").star();
    RegDFA dfa(expr, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == first_state.id);
}

TEST_CASE("RegDFA generic expr", "[RegDFA]") {
    auto expr = (RegPosSets("a").or("b").star())
                .concat("a")
                .concat("b")
                .concat("b");
    RegDFA dfa(expr, IMPOSSIBLE_POS);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);

    auto second_state = dfa.get_current_state();

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == first_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == first_state.id);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    auto third_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == third_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != third_state.id);

    auto final_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == second_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == third_state.id);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == final_state.id);
}

TEST_CASE("RegDFA Test final set", "[RegDFA]") {
    RegPosSets final_pos_sets("F");
    auto expr = (
        (RegPosSets("a").concat("b"))
        .or(RegPosSets("a").concat("c"))
    ).concat(final_pos_sets);

    auto final_pos = *final_pos_sets.first().cbegin();

    RegDFA dfa(expr, final_pos);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);
    CHECK(!first_state.is_final);

    auto second_state = dfa.get_current_state();
    CHECK(!second_state.is_final);

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    auto third_state = dfa.get_current_state();
    CHECK(third_state.is_final);

    dfa.set_current_state(first_state);
    dfa.do_transition("a");

    CHECK(dfa.do_transition("c") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    auto fourth_state = dfa.get_current_state();
    CHECK(fourth_state.is_final);
}
