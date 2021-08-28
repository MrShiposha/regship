#include "regmatcher.hpp"
#include "regsymstream.hpp"

constexpr RegSymbol NOT_SYMBOL("\xff\xff");

static RegDFA mk_dfa(const RegPosSets &rps) {
    RegPosSets final_sets(NOT_SYMBOL);
    auto final_pos = *final_sets.first().cbegin();

    return RegDFA(rps.concat(final_sets), final_pos);
}

RegMatcher::RegMatcher(const RegPosSets &rps)
    : dfa(mk_dfa(rps))
{
    first_state = dfa.get_current_state();
}

bool RegMatcher::is_matches(const std::string_view str) {
    reset();

    RegSymStream stream(str);
    RegSymbol sym;

    while (stream) {
        stream >> sym;
        if (dfa.do_transition(sym) != RegDFA::SUCCESS) {
            return false;
        }
    }

    return dfa.get_current_state().is_final;
}

void RegMatcher::reset() {
    dfa.set_current_state(first_state);
}
