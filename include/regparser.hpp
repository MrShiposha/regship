#ifndef ___REGSHIP_REGPARSER_HPP___
#define ___REGSHIP_REGPARSER_HPP___

#include <memory>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <stack>

#include "regmatcher.hpp"
#include "impl/regparser.hpp"

class RegParser final {
public:
    using GrammarSymbol = regship_impl::RegParserGrammarSymbol;

    RegParser();
    RegParser(const RegParser &) = delete;
    RegParser(RegParser &&) = delete;
    RegParser &operator=(const RegParser &) = delete;
    RegParser &operator=(RegParser &&) = delete;

    ~RegParser() = default;

    RegMatcher parse(const std::string_view);

    bool is_terminal(GrammarSymbol) const;

private:
    using StateTransition = regship_impl::RegParserStateTransition;
    using ProductionBody  = std::vector<GrammarSymbol>;

    void setup_transitions();
    void setup_expr_transitions();
    void setup_or_transitions();
    void setup_or_rest_transitions();
    void setup_ct_transitions();
    void setup_ct_rest_transitions();
    void setup_term_transitions();
    void setup_atom_transitions();
    void setup_star_transitions();

    static ProductionBody EMPTY;

    std::unordered_map<StateTransition, ProductionBody> ll_table;
    std::stack<GrammarSymbol> state_stack;
};

#endif // ___REGSHIP_REGPARSER_HPP___
