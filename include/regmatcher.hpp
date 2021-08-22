#ifndef ___REGSHIP_REGMATCHER_HPP___
#define ___REGSHIP_REGMATCHER_HPP___

#include "regdfa.hpp"

class RegMatcher final {
public:
    RegMatcher() = default;
    RegMatcher(const RegMatcher &) = default;
    RegMatcher(RegMatcher &&) = default;
    RegMatcher &operator=(const RegMatcher &) = default;
    RegMatcher &operator=(RegMatcher &&) = default;

    ~RegMatcher() = default;

private:
    // RegDFA dfa;
};

#endif // ___REGSHIP_REGMATCHER_HPP___
