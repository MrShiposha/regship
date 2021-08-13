#ifndef ___REGSHIP_REGPOSSETS_HPP___
#define ___REGSHIP_REGPOSSETS_HPP___

#include <unordered_set>
#include <unordered_map>
#include <string_view>

using RegSymbol = std::string_view;

class RegPosSets final {
public:
    using SymbolPos = size_t;
    using SymbolPosSet = std::unordered_set<SymbolPos>;

    RegPosSets() = delete;
    RegPosSets &operator=(const RegPosSets &) = delete;
    RegPosSets &operator=(RegPosSets &&) = delete;

    ~RegPosSets() = default;

    RegPosSets(const char *reg_symbol);
    RegPosSets(const RegSymbol);

    RegPosSets concat(const RegPosSets &) const;
    RegPosSets or(const RegPosSets &) const;
    RegPosSets star() const;

    const SymbolPosSet &first() const;
    const SymbolPosSet &last() const;
    const SymbolPosSet &follow(SymbolPos) const;
    bool is_nullable() const;

private:
    RegPosSets(const RegPosSets &) = default;
    RegPosSets(RegPosSets &&) = default;

    void set_follow_pos(SymbolPos, const SymbolPosSet &);
    void append_required_info(const RegPosSets &);

    SymbolPosSet first_set, last_set;
    std::unordered_map<SymbolPos, SymbolPosSet> follow_set;
    std::unordered_map<RegSymbol, SymbolPosSet> symbol_occurrences;

    bool is_nullable_flag;
};

#endif // ___REGSHIP_REGPOSSETS_HPP___
