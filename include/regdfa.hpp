#ifndef ___REGSHIP_REGDFA_HPP___
#define ___REGSHIP_REGDFA_HPP___

#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "regpossets.hpp"

class RegDFA {
public:
    struct PosAction {
        RegPosSets::SymbolPos pos;
        std::function<void()> action;
    };

    enum class TransitionResult {
        SUCCESS,
        DEADLOCK
    };

    RegDFA() = delete;
    RegDFA(const RegDFA &) = delete;
    RegDFA(RegDFA &&) = delete;
    RegDFA &operator=(const RegDFA &) = delete;
    RegDFA &operator=(RegDFA &&) = delete;

    RegDFA(const RegPosSets &);

    ~RegDFA() = default;

    TransitionResult do_transition(RegPosSets::SymbolPos);

    void set_pos_action(RegPosSets::SymbolPos, PosAction);

    std::unordered_set<PosAction> current_pos_actions();

private:
    using State              = size_t;
    using StateAndTransition = size_t;

    StateAndTransition make_dfa_table_key(const RegSymbol);

    std::unordered_map<StateAndTransition, State> dfa_table;
    std::unordered_map<State, PosAction> pos_actions;
    State current_state;
};

#endif // ___REGSHIP_REGDFA_HPP___
