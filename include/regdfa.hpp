#ifndef ___REGSHIP_REGDFA_HPP___
#define ___REGSHIP_REGDFA_HPP___

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "regpossets.hpp"
#include "impl/regdfa.hpp"

class RegDFA final {
public:
    using StateId         = regship_impl::RegDfaStateId;
    using StateTransition = regship_impl::RegDfaStateTransition;

    struct State {
        StateId id;
        bool is_final;
    };

    enum TransitionResult {
        SUCCESS,
        DEADSTATE
    };

    RegDFA() = delete;
    RegDFA(const RegDFA &) = delete;
    RegDFA(RegDFA &&) = default;
    RegDFA &operator=(const RegDFA &) = delete;
    RegDFA &operator=(RegDFA &&) = delete;

    RegDFA(const RegPosSets &, const RegPosSets::SymbolPos &final_pos);

    ~RegDFA() = default;

    TransitionResult do_transition(const RegSymbol);

    const State &get_current_state() const;

    void set_current_state(const State &);

private:
    std::unordered_map<StateTransition, State> dfa_table;
    State dfa_current_state;
};

#endif // ___REGSHIP_REGDFA_HPP___
