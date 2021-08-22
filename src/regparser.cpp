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
    state_stack = std::stack<GrammarSymbol>();
    state_stack.push(GrammarSymbol::END);
    state_stack.push(GrammarSymbol::expr);

    TokenStream stream(str);
    Token cur_token;
    GrammarSymbol cur_grammar_sym;

    stream >> cur_token;

    while ((cur_grammar_sym = state_stack.top()) != GrammarSymbol::END) {
        if (cur_grammar_sym == cur_token.kind) {
            state_stack.pop();
            stream >> cur_token;
            continue;
        } else if (is_terminal(cur_grammar_sym)) {
            throw_unexpected_token(cur_token);
        }

        auto state_transition = StateTransition(cur_grammar_sym, cur_token.kind);
        auto prod_it = ll_table.find(state_transition);
        if (prod_it == ll_table.end()) {
            throw_unexpected_token(cur_token);
        }

        state_stack.pop();

        auto prod_body_it = prod_it->second.crbegin();
        auto prod_end = prod_it->second.crend();
        for (; prod_body_it != prod_end; ++prod_body_it) {
            state_stack.push(*prod_body_it);
        }
    }

    if (cur_token.kind != GrammarSymbol::END) {
        throw_unexpected_token(cur_token);
    }

    return RegMatcher();
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

void RegParser::setup_expr_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::SYMBOL),
        ProductionBody { GrammarSymbol::or }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::OPEN_PAREN),
        ProductionBody { GrammarSymbol::or }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::expr, GrammarSymbol::END),
        EMPTY
    );
}

void RegParser::setup_or_transitions() {
    ProductionBody body = { GrammarSymbol::ct, GrammarSymbol::or_rest };

    ll_table.emplace(
        StateTransition(GrammarSymbol::or, GrammarSymbol::SYMBOL),
        body
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or, GrammarSymbol::OPEN_PAREN),
        body
    );
}

void RegParser::setup_or_rest_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::CLOSE_PAREN),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::END),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::or_rest, GrammarSymbol::OR),
        ProductionBody { GrammarSymbol::OR, GrammarSymbol::ct, GrammarSymbol::or_rest }
    );
}

void RegParser::setup_ct_transitions() {
    ProductionBody body = { GrammarSymbol::term, GrammarSymbol::ct_rest };

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct, GrammarSymbol::SYMBOL),
        body
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct, GrammarSymbol::OPEN_PAREN),
        body
    );
}

void RegParser::setup_ct_rest_transitions() {
    ProductionBody body = { GrammarSymbol::term, GrammarSymbol::ct_rest };

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::SYMBOL),
        body
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::OPEN_PAREN),
        body
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::CLOSE_PAREN),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::OR),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::ct_rest, GrammarSymbol::END),
        EMPTY
    );
}

void RegParser::setup_term_transitions() {
    ProductionBody body = { GrammarSymbol::atom, GrammarSymbol::star };

    ll_table.emplace(
        StateTransition(GrammarSymbol::term, GrammarSymbol::SYMBOL),
        body
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::term, GrammarSymbol::OPEN_PAREN),
        body
    );
}

void RegParser::setup_atom_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::atom, GrammarSymbol::SYMBOL),
        ProductionBody { GrammarSymbol::SYMBOL }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::atom, GrammarSymbol::OPEN_PAREN),
        ProductionBody {
            GrammarSymbol::OPEN_PAREN,
            GrammarSymbol::expr,
            GrammarSymbol::CLOSE_PAREN
        }
    );
}

void RegParser::setup_star_transitions() {
    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::STAR),
        ProductionBody { GrammarSymbol::STAR }
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::SYMBOL),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::OPEN_PAREN),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::CLOSE_PAREN),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::OR),
        EMPTY
    );

    ll_table.emplace(
        StateTransition(GrammarSymbol::star, GrammarSymbol::END),
        EMPTY
    );
}

RegParser::ProductionBody RegParser::EMPTY;
