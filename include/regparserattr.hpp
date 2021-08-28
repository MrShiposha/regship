#ifndef ___REGSHIP_REGPARSERATTR_HPP___
#define ___REGSHIP_REGPARSERATTR_HPP___

#include <memory>
#include <functional>
#include <variant>

#include "regpossets.hpp"

class RegParserAttr : public std::enable_shared_from_this<RegParserAttr> {
public:
    using Value = RegPosSets;
    using Gen   = std::function<RegParserAttr::Value()>;

    RegParserAttr(const RegParserAttr &) = delete;
    RegParserAttr(RegParserAttr &&) = delete;
    RegParserAttr &operator=(const RegParserAttr &) = delete;
    RegParserAttr &operator=(RegParserAttr &&) = delete;

    ~RegParserAttr() = default;

    virtual const Value &value();

    void set_value(const Value &);

    std::shared_ptr<RegParserAttr> defer_concat(const std::shared_ptr<RegParserAttr>);
    std::shared_ptr<RegParserAttr> defer_or(const std::shared_ptr<RegParserAttr>);
    // void defer_star_inplace();

    static std::shared_ptr<RegParserAttr> mk_attr();

private:
    RegParserAttr();
    RegParserAttr(Gen);

    std::variant<Value, Gen> data;
};

// class RegParserBasicAttr : public RegParserAttr {
// public:
//     using RegParserAttr::RegParserAttr;

//     RegParserBasicAttr(const RegParserBasicAttr &) = default;
//     RegParserBasicAttr(RegParserBasicAttr &&) = default;
//     RegParserBasicAttr &operator=(const RegParserBasicAttr &) = default;
//     RegParserBasicAttr &operator=(RegParserBasicAttr &&) = default;

//     ~RegParserBasicAttr() = default;

//     virtual const Value &value() override;
// };

// class RegParserDeferAttr : public RegParserAttr {
// public:
//     using Gen = std::function<RegParserAttr::Value()>;

//     RegParserDeferAttr(Gen);

//     RegParserDeferAttr(const RegParserDeferAttr &) = default;
//     RegParserDeferAttr(RegParserDeferAttr &&) = default;
//     RegParserDeferAttr &operator=(const RegParserDeferAttr &) = default;
//     RegParserDeferAttr &operator=(RegParserDeferAttr &&) = default;

//     ~RegParserDeferAttr() = default;

//     virtual const Value &value() override;
// private:
//     Gen gen;
//     bool is_valid;
// };

#endif // ___REGSHIP_REGPARSERATTR_HPP___
