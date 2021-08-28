#include <functional>
#include <stdexcept>

#include "regparser.hpp"
#include "regsymstream.hpp"

using namespace std::string_literals;

struct Token {
    Token()
        : kind(RegParser::GrammarSymbol::END), symbol("")
    {}

    Token(RegParser::GrammarSymbol kind, RegSymbol sym)
        : kind(kind), symbol(sym)
    {}

    RegParser::GrammarSymbol kind;
    RegSymbol symbol;
};


class TokenStream final {
public:
    TokenStream(const std::string_view str)
        : stream(str)
    {}

    TokenStream &operator>>(Token &out) {
        RegSymbol sym;
        bool is_esc_seq = false;

        stream >> sym;
        if (sym == "\\") {
            stream >> sym;
            is_esc_seq = true;
            out = to_token(sym, is_esc_seq);
        } else {
            is_esc_seq = false;
            out = to_token(sym, is_esc_seq);
        }

        return *this;
    }

private:
    Token to_token(const RegSymbol sym, bool is_esc_seq) {
        static Token open_tok (RegParser::GrammarSymbol::OPEN_PAREN , "(");
        static Token close_tok(RegParser::GrammarSymbol::CLOSE_PAREN, ")");
        static Token star_tok (RegParser::GrammarSymbol::STAR       , "*");
        static Token or_tok   (RegParser::GrammarSymbol::OR         , "|");
        static Token inv_tok  (RegParser::GrammarSymbol::INVALID    , "" );
        static Token end_tok  (RegParser::GrammarSymbol::END        , "" );

        auto mk_sym_tok = [&] { return Token(RegParser::GrammarSymbol::SYMBOL, sym); };

        if (sym.empty()) {
            return (is_esc_seq ? inv_tok : end_tok);
        } else if (sym == "(") {
            return (is_esc_seq ? mk_sym_tok() : open_tok);
        } else if (sym == ")") {
            return (is_esc_seq ? mk_sym_tok() : close_tok);
        } else if (sym == "*") {
            return (is_esc_seq ? mk_sym_tok() : star_tok);
        } else if (sym == "|") {
            return (is_esc_seq ? mk_sym_tok() : or_tok);
        } else {
            return (is_esc_seq ? inv_tok : mk_sym_tok());
        }
    }

    RegSymStream stream;
};

RegParser::RegParser()
{
    setup_transitions();
}

void throw_unexpected_token(const Token &token) {
    throw std::invalid_argument(
        "unexpected "s + (
            token.kind == RegParser::GrammarSymbol::END
            ? "end of a regular expression"s
            : "symbol `"s + token.symbol.data() + "`"
        )
    );
}

RegMatcher RegParser::parse(const std::string_view str) {
    symbol_stack = std::stack<AnnotatedGrammarSymbol>();

    AnnotatedGrammarSymbol start_symbol(GrammarSymbol::expr);

    symbol_stack.push(start_symbol);

    TokenStream stream(str);
    Token cur_token;

    stream >> cur_token;

    while (!symbol_stack.empty()) {
        AnnotatedGrammarSymbol cur_grammar_sym = symbol_stack.top();

        if (cur_grammar_sym.kind == cur_token.kind) {
            cur_grammar_sym.syn_attr->set_value(cur_token.symbol);

            symbol_stack.pop();
            stream >> cur_token;
            continue;
        } else if (is_terminal(cur_grammar_sym.kind)) {
            throw_unexpected_token(cur_token);
        }

        auto state_transition = StateTransition(cur_grammar_sym.kind, cur_token.kind);
        auto prod_it = ll_table.find(state_transition);
        if (prod_it == ll_table.end()) {
            throw_unexpected_token(cur_token);
        }

        symbol_stack.pop();

        auto &prod_expansion = prod_it->second;
        prod_expansion(cur_grammar_sym);
    }

    if (cur_token.kind != GrammarSymbol::END) {
        throw_unexpected_token(cur_token);
    }

    return RegMatcher(start_symbol.syn_attr->value());
}

bool RegParser::is_terminal(GrammarSymbol symbol) const {
    switch (symbol) {
    case GrammarSymbol::SYMBOL:
    case GrammarSymbol::OPEN_PAREN:
    case GrammarSymbol::CLOSE_PAREN:
    case GrammarSymbol::STAR:
    case GrammarSymbol::OR:
    case GrammarSymbol::END:
        return true;

    default:
        return false;
    }
}

void RegParser::setup_transitions() {
    setup_expr_transitions();
    setup_or_transitions();
    setup_or_rest_transitions();
    setup_ct_transitions();
    setup_ct_rest_transitions();
    setup_term_transitions();
    setup_atom_transitions();
    setup_star_transitions();
}

void RegParser::empty_prod_expansion(AnnotatedGrammarSymbol &) {}

void RegParser::inh_to_syn_prod_expansion(AnnotatedGrammarSymbol &sym) {
    sym.syn_attr->set_value(sym.inh_attr->value());
}

void RegParser::setup_expr_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol sym(GrammarSymbol::or);
        sym.inh_attr = header.inh_attr;
        sym.syn_attr = header.syn_attr;

        symbol_stack.push(sym);
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::SYMBOL),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::OPEN_PAREN),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::END),
        empty_prod_expansion
    );
}

void RegParser::setup_or_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol ct_sym(GrammarSymbol::ct);
        AnnotatedGrammarSymbol or_rest_sym(GrammarSymbol::or_rest);

        ct_sym.inh_attr = header.inh_attr;
        or_rest_sym.inh_attr = ct_sym.syn_attr;
        or_rest_sym.syn_attr = header.syn_attr;

        symbol_stack.push(or_rest_sym);
        symbol_stack.push(ct_sym);
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::or, GrammarSymbol::SYMBOL),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or, GrammarSymbol::OPEN_PAREN),
        prod_expansion
    );
}

void RegParser::setup_or_rest_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol ct_sym(GrammarSymbol::ct);
        AnnotatedGrammarSymbol or_rest_sym(GrammarSymbol::or_rest);

        or_rest_sym.inh_attr = header.inh_attr->defer_or(ct_sym.syn_attr);
        or_rest_sym.syn_attr = header.syn_attr;

        symbol_stack.push(or_rest_sym);
        symbol_stack.push(ct_sym);
        symbol_stack.push({ GrammarSymbol::OR });
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::OR),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::CLOSE_PAREN),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::END),
        inh_to_syn_prod_expansion
    );
}

void RegParser::setup_ct_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol term_sym(GrammarSymbol::term);
        AnnotatedGrammarSymbol ct_rest_sym(GrammarSymbol::ct_rest);

        ct_rest_sym.inh_attr = header.inh_attr->defer_concat(term_sym.syn_attr);
        ct_rest_sym.syn_attr = header.syn_attr;

        symbol_stack.push(ct_rest_sym);
        symbol_stack.push(term_sym);
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct, GrammarSymbol::SYMBOL),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct, GrammarSymbol::OPEN_PAREN),
        prod_expansion
    );
}

void RegParser::setup_ct_rest_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol term_sym(GrammarSymbol::term);
        AnnotatedGrammarSymbol ct_rest_sym(GrammarSymbol::ct_rest);

        ct_rest_sym.inh_attr = header.inh_attr->defer_concat(term_sym.syn_attr);
        ct_rest_sym.syn_attr = header.syn_attr;

        symbol_stack.push(ct_rest_sym);
        symbol_stack.push(term_sym);
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::SYMBOL),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::OPEN_PAREN),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::CLOSE_PAREN),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::OR),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::END),
        inh_to_syn_prod_expansion
    );
}

void RegParser::setup_term_transitions() {
    auto prod_expansion = [&](auto header) {
        AnnotatedGrammarSymbol atom_sym(GrammarSymbol::atom);
        AnnotatedGrammarSymbol star_sym(GrammarSymbol::star);

        star_sym.inh_attr = atom_sym.syn_attr;
        star_sym.syn_attr = header.syn_attr;

        symbol_stack.push(star_sym);
        symbol_stack.push(atom_sym);
    };

    ll_table.emplace(
        StateTransition(GrammarSymbol::term, GrammarSymbol::SYMBOL),
        prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::term, GrammarSymbol::OPEN_PAREN),
        prod_expansion
    );
}

void RegParser::setup_atom_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::atom, GrammarSymbol::SYMBOL),
        [&](auto header) {
            AnnotatedGrammarSymbol sym(GrammarSymbol::SYMBOL);
            sym.syn_attr = header.syn_attr;

            symbol_stack.push(sym);
        }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::atom, GrammarSymbol::OPEN_PAREN),
        [&](auto header) {
            AnnotatedGrammarSymbol expr_sym(GrammarSymbol::expr);
            expr_sym.syn_attr = header.syn_attr;

            symbol_stack.push({ GrammarSymbol::CLOSE_PAREN });
            symbol_stack.push(expr_sym);
            symbol_stack.push({ GrammarSymbol::OPEN_PAREN });
        }
    );
}

void RegParser::setup_star_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::STAR),
        [&](auto header) {
            header.syn_attr->set_value(header.inh_attr->value().star());

            symbol_stack.push({ GrammarSymbol::STAR });
        }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::SYMBOL),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::OPEN_PAREN),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::CLOSE_PAREN),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::OR),
        inh_to_syn_prod_expansion
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::END),
        inh_to_syn_prod_expansion
    );
}

RegParser::AnnotatedGrammarSymbol::AnnotatedGrammarSymbol(GrammarSymbol kind)
    : kind(kind),
    inh_attr(Attr::mk_attr()),
    syn_attr(Attr::mk_attr())
{}
