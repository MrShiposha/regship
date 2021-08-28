#ifndef ___REGSHIP_REGPOSSETS_HPP___
#define ___REGSHIP_REGPOSSETS_HPP___

#include <unordered_set>
#include <unordered_map>

#include "regsymbol.hpp"

class RegPosSets final {
public:
    using SymbolPos = size_t;
    using SymbolPosSet = std::unordered_set<SymbolPos>;

    RegPosSets() = default;
    RegPosSets(const RegPosSets &) = default;
    RegPosSets(RegPosSets &&) = default;

    RegPosSets &operator=(const RegPosSets &) = default;
    RegPosSets &operator=(RegPosSets &&) = default;

    ~RegPosSets() = default;

    RegPosSets(const char *reg_symbol);
    RegPosSets(const RegSymbol);

    RegPosSets concat(const RegPosSets &) const;
    RegPosSets or(const RegPosSets &) const;
    RegPosSets star() const;
    RegPosSets nullable() const;

    void concat_inplace(const RegPosSets &);
    void or_inplace(const RegPosSets &);
    void star_inplace();
    void nullable_inplace();

    const SymbolPosSet &first() const;
    const SymbolPosSet &last() const;
    const SymbolPosSet &follow(SymbolPos) const;
    bool is_nullable() const;

    const std::unordered_map<RegSymbol, SymbolPosSet> &symbol_occurrences() const;

private:
    void set_follow_pos(SymbolPos, const SymbolPosSet &);
    void append_required_info(const RegPosSets &);

    SymbolPosSet first_set, last_set;
    std::unordered_map<SymbolPos, SymbolPosSet> follow_set;
    std::unordered_map<RegSymbol, SymbolPosSet> symbol_occurrences_set;

    bool is_nullable_flag;
};

#endif // ___REGSHIP_REGPOSSETS_HPP___
