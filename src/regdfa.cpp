#include <stack>

#include "regdfa.hpp"

RegDFA::RegDFA(const RegPosSets &pos_sets) {
    // TODO
    // std::hash<RegPosSets::SymbolPosSet> hasher {};
    // RegPosSets::SymbolPosSet new_set_state, follow_pos;

    // std::stack<State> unvisited;

    // auto first_state = hasher(pos_sets.first());
    // unvisited.push(first_state);

    // do {
    //     auto state = unvisited.pop();
    //     new_set_state.clear();

    //     for (auto &&[symbol, sym_pos_set] : pos_sets.symbol_occurrences()) {
    //         // new_set_state.insert();
    //     }
    // } while (!unvisited.empty());
}

RegDFA::TransitionResult RegDFA::do_transition(RegPosSets::SymbolPos) {

}

void RegDFA::set_pos_action(RegPosSets::SymbolPos, RegDFA::PosAction) {

}

std::unordered_set<RegDFA::PosAction> RegDFA::current_pos_actions() {

}
