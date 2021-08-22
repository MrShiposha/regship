#ifndef ___REGSHIP_IMPL_REGPARSER_HPP___
#define ___REGSHIP_IMPL_REGPARSER_HPP___

#include "util.hpp"

namespace regship_impl {
    enum class RegParserGrammarSymbol {
        // Terminals
        SYMBOL,      // <symbol>
        OPEN_PAREN,  // '('
        CLOSE_PAREN, // ')'
        STAR,        // '*'
        OR,          // '|'
        END,

        // Non-terminals
        expr,    // expr    -> or
        or,      // or      -> ct or_rest
        or_rest, // or_rest -> '|' ct or_rest | <empty>
        ct,      // ct      -> term ct_rest
        ct_rest, // ct_rest -> term ct_rest
        term,    // term    -> atom star
        atom,    // atom    -> <symbol> | '(' expr ')'
        star,    // star    -> '*' | <empty>

        // Special value
        INVALID,
        ENUM_LAST_VAL
    };

    using RegParserStateTransition = std::pair<
        RegParserGrammarSymbol,
        RegParserGrammarSymbol
    >;
}

namespace std {
    template <>
    struct hash<regship_impl::RegParserStateTransition> {
        size_t operator()(const regship_impl::RegParserStateTransition &transition) const {
            using Hasher = std::hash<regship_impl::RegParserGrammarSymbol>;

            Hasher hasher;
            return combine_hashes(hasher(transition.first), hasher(transition.second));
        }
    };
}

#endif // ___REGSHIP_IMPL_REGPARSER_HPP___
