#include <atomic>

#include "regpossets.hpp"

static std::atomic<RegPosSets::SymbolPos> unique_pos(0);

static RegPosSets::SymbolPos next_unique_pos() {
    return unique_pos++;
}

template <typename T>
static void append_to_left(T &out, const T &append) {
    out.insert(append.cbegin(), append.cend());
}

RegPosSets::RegPosSets(const char *reg_symbol)
    : RegPosSets(RegSymbol(reg_symbol))
{}

RegPosSets::RegPosSets(const RegSymbol symbol)
    : first_set(), last_set(), follow_set(), symbol_occurrences(), is_nullable_flag(false)
{
    auto symbol_pos = next_unique_pos();

    SymbolPosSet one_symbol_set = { symbol_pos };
    first_set = one_symbol_set;
    last_set = one_symbol_set;
    follow_set.emplace(symbol_pos, SymbolPosSet());

    symbol_occurrences.emplace(symbol, symbol_pos);
}

RegPosSets RegPosSets::concat(const RegPosSets &other) const {
    auto copy = *this;

    copy.is_nullable_flag = copy.is_nullable_flag && other.is_nullable_flag;
    if (this->is_nullable_flag) {
        append_to_left(
            copy.first_set,
            other.first_set
        );
    }

    if (!other.is_nullable_flag) {
        copy.last_set = other.last_set;
    } else {
        append_to_left(
            copy.last_set,
            other.last_set
        );
    }

    for (auto &&left_last_pos : last_set) {
        copy.set_follow_pos(left_last_pos, other.first_set);
    }

    copy.append_required_info(other);

    return copy;
}

RegPosSets RegPosSets::or(const RegPosSets &other) const {
    auto copy = *this;

    copy.is_nullable_flag = copy.is_nullable_flag || other.is_nullable_flag;
    append_to_left(
        copy.first_set,
        other.first_set
    );

    append_to_left(
        copy.last_set,
        other.last_set
    );

    copy.append_required_info(other);

    return copy;
}

RegPosSets RegPosSets::star() const {
    auto copy = *this;
    copy.is_nullable_flag = true;

    for (auto &&first_pos : copy.first_set) {
        copy.set_follow_pos(first_pos, copy.last_set);
    }

    return copy;
}

const RegPosSets::SymbolPosSet &RegPosSets::first() const {
    return first_set;
}

const RegPosSets::SymbolPosSet &RegPosSets::last() const {
    return last_set;
}

const RegPosSets::SymbolPosSet &RegPosSets::follow(SymbolPos pos) const {
    static SymbolPosSet empty_set;

    auto it = follow_set.find(pos);
    if (it != follow_set.end()) {
        return it->second;
    } else {
        return empty_set;
    }
}

bool RegPosSets::is_nullable() const {
    return is_nullable_flag;
}

void RegPosSets::set_follow_pos(
    RegPosSets::SymbolPos current_pos,
    const RegPosSets::SymbolPosSet &extra_follow_set
) {
    auto &&[it, was_inserted] = follow_set.try_emplace(current_pos, extra_follow_set);
    if (!was_inserted) {
        append_to_left(it->second, extra_follow_set);
    }
}

void RegPosSets::append_required_info(const RegPosSets &other) {
    append_to_left(
        symbol_occurrences,
        other.symbol_occurrences
    );

    append_to_left(
        follow_set,
        other.follow_set
    );
}