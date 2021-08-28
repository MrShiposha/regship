#ifndef ___REGSHIP_REGPARSER_HPP___
#define ___REGSHIP_REGPARSER_HPP___

#include <memory>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <stack>
#include <functional>

#include "regpossets.hpp"
#include "regmatcher.hpp"
#include "regparserattr.hpp"
#include "impl/regparser.hpp"

class RegParser final {
public:
    using GrammarSymbol = regship_impl::RegParserGrammarSymbol;
    using Attr          = RegParserAttr;
    using AttrPtr       = std::shared_ptr<Attr>;

    RegParser();
    RegParser(const RegParser &) = delete;
    RegParser(RegParser &&) = delete;
    RegParser &operator=(const RegParser &) = delete;
    RegParser &operator=(RegParser &&) = delete;

    ~RegParser() = default;

    RegMatcher parse(const std::string_view);

    bool is_terminal(GrammarSymbol) const;

private:
    struct AnnotatedGrammarSymbol {
        AnnotatedGrammarSymbol(GrammarSymbol);

        GrammarSymbol kind;
        AttrPtr inh_attr, syn_attr;
    };

    using StateTransition  = regship_impl::RegParserStateTransition;
    using ProductionAction = std::function<void(AnnotatedGrammarSymbol &)>;

    void setup_transitions();
    void setup_expr_transitions();
    void setup_or_transitions();
    void setup_or_rest_transitions();
    void setup_ct_transitions();
    void setup_ct_rest_transitions();
    void setup_term_transitions();
    void setup_atom_transitions();
    void setup_star_transitions();

    static void empty_prod_expansion(AnnotatedGrammarSymbol &);
    static void inh_to_syn_prod_expansion(AnnotatedGrammarSymbol &);

    std::unordered_map<StateTransition, ProductionAction> ll_table;
    std::stack<AnnotatedGrammarSymbol> symbol_stack;
};

#endif // ___REGSHIP_REGPARSER_HPP___
