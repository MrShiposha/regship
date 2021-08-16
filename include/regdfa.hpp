#ifndef ___REGSHIP_REGDFA_HPP___
#define ___REGSHIP_REGDFA_HPP___

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "regpossets.hpp"
#include "util.hpp"

namespace regship_impl {
    using StateId = size_t;
    using StateTransition = std::pair<StateId, RegSymbol>;
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

class RegDFA final {
public:
    using StateId         = regship_impl::StateId;
    using StateTransition = regship_impl::StateTransition;

    struct State {
        StateId id;
        std::shared_ptr<RegPosSets::SymbolPosSet> final_pos_set;
    };

    enum TransitionResult {
        SUCCESS,
        DEADSTATE
    };

    RegDFA() = delete;
    RegDFA(const RegDFA &) = delete;
    RegDFA(RegDFA &&) = delete;
    RegDFA &operator=(const RegDFA &) = delete;
    RegDFA &operator=(RegDFA &&) = delete;

    RegDFA(const RegPosSets &, const RegPosSets::SymbolPosSet &final_pos_set);

    ~RegDFA() = default;

    TransitionResult do_transition(const RegSymbol);

    const State &get_current_state() const;

    void set_current_state(const State &);

private:
    std::unordered_map<StateTransition, State> dfa_table;
    State dfa_current_state;
};

#endif // ___REGSHIP_REGDFA_HPP___
