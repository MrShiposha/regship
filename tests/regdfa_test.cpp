#include "catch2/catch_test_macros.hpp"

#include "regdfa.hpp"

static RegPosSets::SymbolPosSet EMPTY_SET;

TEST_CASE("RegDFA Test empty", "[RegDFA]") {
    RegPosSets empty("");
    RegDFA dfa(empty, EMPTY_SET);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
    CHECK(dfa.do_transition("<not existing symbol>") == RegDFA::DEADSTATE);
    REQUIRE(dfa.get_current_state().id == first_state.id);
}

TEST_CASE("RegDFA Test one symbol", "[RegDFA]") {
    RegPosSets a_pos_sets("a");
    RegDFA dfa(a_pos_sets, EMPTY_SET);

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
        RegDFA dfa(concat, EMPTY_SET);

        auto first_state = dfa.get_current_state();

        CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
        REQUIRE(dfa.get_current_state().id != first_state.id);

        auto second_state = dfa.get_current_state();

        CHECK(dfa.do_transition("b") == RegDFA::DEADSTATE);
        REQUIRE(dfa.get_current_state().id == second_state.id);
    }

    {
        auto concat = RegPosSets("a").concat("b").concat("b");
        RegDFA dfa(concat, EMPTY_SET);

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
    RegDFA dfa(expr, EMPTY_SET);

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
    RegDFA dfa(expr, EMPTY_SET);

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
    RegDFA dfa(expr, EMPTY_SET);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id == first_state.id);
}

TEST_CASE("RegDFA generic expr", "[RegDFA]") {
    auto expr = (RegPosSets("a").or("b").star())
                .concat("a")
                .concat("b")
                .concat("b");
    RegDFA dfa(expr, EMPTY_SET);

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
    RegPosSets final_1("c"), final_2("b");
    auto expr = (RegPosSets("a").concat("b").concat(final_1))
                .or(RegPosSets("a").concat("c").concat(final_2));

    auto final_1_pos = *final_1.first().cbegin();
    auto final_2_pos = *final_2.first().cbegin();

    RegPosSets::SymbolPosSet final_set = { final_1_pos, final_2_pos };

    RegDFA dfa(expr, final_set);

    auto first_state = dfa.get_current_state();

    CHECK(dfa.do_transition("a") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != first_state.id);
    CHECK(first_state.final_pos_set->empty());

    auto second_state = dfa.get_current_state();
    CHECK(second_state.final_pos_set->empty());

    CHECK(dfa.do_transition("b") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    auto third_state = dfa.get_current_state();
    CHECK(third_state.final_pos_set->count(final_1_pos) > 0);

    dfa.set_current_state(first_state);
    dfa.do_transition("a");

    CHECK(dfa.do_transition("c") == RegDFA::SUCCESS);
    REQUIRE(dfa.get_current_state().id != second_state.id);

    auto fourth_state = dfa.get_current_state();
    CHECK(fourth_state.final_pos_set->count(final_2_pos) > 0);
}
