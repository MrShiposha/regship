#ifndef ___REGSHIP_IMPL_REGDFA_HPP___
#define ___REGSHIP_IMPL_REGDFA_HPP___

#include "util.hpp"

namespace regship_impl {
    using RegDfaStateId = size_t;
    using RegDfaStateTransition = std::pair<RegDfaStateId, RegSymbol>;
}

namespace std {
    template <>
    struct hash<regship_impl::RegDfaStateTransition> {
        size_t operator()(const regship_impl::RegDfaStateTransition &transition) const {
            using Hasher = std::hash<RegSymbol>;
            return combine_hashes(transition.first, Hasher{}(transition.second));
        }
    };
}

#endif // ___REGSHIP_IMPL_REGDFA_HPP___
