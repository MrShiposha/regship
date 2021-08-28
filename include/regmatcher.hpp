#ifndef ___REGSHIP_REGMATCHER_HPP___
#define ___REGSHIP_REGMATCHER_HPP___

#include <string_view>

#include "regpossets.hpp"
#include "regdfa.hpp"

class RegMatcher final {
public:
    RegMatcher(const RegPosSets &);

    RegMatcher() = delete;
    RegMatcher(const RegMatcher &) = delete;
    RegMatcher(RegMatcher &&) = delete;
    RegMatcher &operator=(const RegMatcher &) = delete;
    RegMatcher &operator=(RegMatcher &&) = delete;

    ~RegMatcher() = default;

    bool is_matches(const std::string_view);

private:
    void reset();

    RegDFA dfa;
    RegDFA::State first_state;
};

#endif // ___REGSHIP_REGMATCHER_HPP___
