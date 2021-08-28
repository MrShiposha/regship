#ifndef ___REGSHIP_IMPL_REGPARSER_HPP___
#define ___REGSHIP_IMPL_REGPARSER_HPP___

#include <functional>
#include <cassert>

#include "util.hpp"
#include "regsymbol.hpp"

class RegPosSets;

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
        INVALID
    };

    using RegParserStateTransition = std::pair<
        RegParserGrammarSymbol,
        RegParserGrammarSymbol
    >;

    class RegParserGrammarSymbolAttribute {
    public:
        using Value    = RegPosSets;
        using ValuePtr = std::shared_ptr<Value>;

        RegParserGrammarSymbolAttribute()
            : RegParserGrammarSymbolAttribute(
                std::make_shared<Value>()
            )
        {}

        RegParserGrammarSymbolAttribute(const RegSymbol symbol)
            : RegParserGrammarSymbolAttribute(
                std::make_shared<Value>(symbol)
            )
        {}

        RegParserGrammarSymbolAttribute(Value &&value)
            : RegParserGrammarSymbolAttribute(
                std::make_shared<Value>(std::forward<Value>(value))
            )
        {}

        RegParserGrammarSymbolAttribute(ValuePtr value_ptr)
            : get_attr_val_fn([] { return Value(); }), value_ptr(value_ptr), is_valid(true)
        {}

        RegParserGrammarSymbolAttribute(std::function<Value()> get_attr_val_fn)
            : get_attr_val_fn(get_attr_val_fn), value_ptr(std::make_shared<Value>()), is_valid(false)
        {}

        RegParserGrammarSymbolAttribute concat(const RegParserGrammarSymbolAttribute other) const {
            auto this_ptr = value_ptr;
            auto other_ptr = other.value_ptr;
            return [this_ptr, other_ptr] { return this_ptr->concat(*other_ptr); };
        }

        RegParserGrammarSymbolAttribute or(const RegParserGrammarSymbolAttribute other) const {
            auto this_ptr = value_ptr;
            auto other_ptr = other.value_ptr;
            return [this_ptr, other_ptr] { return this_ptr->or(*other_ptr); };
        }

        RegParserGrammarSymbolAttribute star() const {
            auto this_ptr = value_ptr;
            return [this_ptr] { return this_ptr->star(); };
        }

        const Value &operator()() {
            if (!is_valid) {
                *value_ptr = get_attr_val_fn();
                is_valid = true;
            }

            return *value_ptr;
        }

    private:
        std::function<Value()> get_attr_val_fn;
        ValuePtr value_ptr;
        bool is_valid;
    };
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
