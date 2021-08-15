#ifndef ___REGSHIP_REGDFA_HPP___
#define ___REGSHIP_REGDFA_HPP___

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "regpossets.hpp"
#include "util.hpp"

namespace regship_impl {
    using State = size_t;
    using StateTransition = std::pair<State, RegSymbol>;
}

namespace std {
    template <>
    struct hash<regship_impl::StateTransition> {
        size_t operator()(const regship_impl::StateTransition &transition) const {
            using Hasher = std::hash<RegSymbol>;
            return combine_hashes(transition.first, Hasher{}(transition.second));
        }
    };
}

class RegDFA {
public:
    struct PosAction {
        RegPosSets::SymbolPos pos;
        std::function<void()> action;
    };

    enum TransitionResult {
        SUCCESS,
        DEADSTATE
    };

    using State           = regship_impl::State;
    using StateTransition = regship_impl::StateTransition;
    using PosActions      = std::vector<PosAction>;

    RegDFA() = delete;
    RegDFA(const RegDFA &) = delete;
    RegDFA(RegDFA &&) = delete;
    RegDFA &operator=(const RegDFA &) = delete;
    RegDFA &operator=(RegDFA &&) = delete;

    RegDFA(const RegPosSets &);

    ~RegDFA() = default;

    TransitionResult do_transition(const RegSymbol);

    void set_pos_action(RegPosSets::SymbolPos, PosAction);

    const PosActions &current_pos_actions() const;

    State current_state() const;

    void set_current_state(State);

private:
    std::unordered_map<StateTransition, State> dfa_table;
    std::unordered_map<State, PosActions> pos_actions;
    State dfa_current_state;
};

#endif // ___REGSHIP_REGDFA_HPP___
