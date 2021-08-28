#include "regparserattr.hpp"

RegParserAttr::RegParserAttr()
    : data(Value())
{}

RegParserAttr::RegParserAttr(Gen gen)
    : data(gen)
{}

const RegParserAttr::Value &RegParserAttr::value() {
    if (std::holds_alternative<Gen>(data)) {
        set_value(std::get<Gen>(data)());
    }

    return std::get<Value>(data);
}

void RegParserAttr::set_value(const Value &value) {
    data = value;
}

std::shared_ptr<RegParserAttr> RegParserAttr::defer_concat(const std::shared_ptr<RegParserAttr> attr) {
    auto thiz = shared_from_this();
    auto other = attr;

    auto concat = new RegParserAttr([=] {
        return thiz->value().concat(other->value());
    });

    return std::shared_ptr<RegParserAttr>(concat);
}

std::shared_ptr<RegParserAttr> RegParserAttr::defer_or(const std::shared_ptr<RegParserAttr> attr) {
    auto thiz = shared_from_this();
    auto other = attr;

    auto or = new RegParserAttr([=] {
        return thiz->value().or(other->value());
    });

    return std::shared_ptr<RegParserAttr>(or);
}

// void RegParserAttr::defer_star_inplace() {
//     Value old_value = value();

//     data = [old_value]() mutable {
//         old_value.star_inplace();

//         return old_value;
//     };
// }

std::shared_ptr<RegParserAttr> RegParserAttr::mk_attr() {
    return std::shared_ptr<RegParserAttr>(new RegParserAttr());
}

// const RegParserAttr::Value &RegParserBasicAttr::value() {
//     return val;
// }

// RegParserDeferAttr::RegParserDeferAttr(Gen gen)
//     : RegParserAttr(), gen(gen), is_valid(false)
// {}

// const RegParserAttr::Value &RegParserDeferAttr::value() {
//     if (!is_valid) {
//         val = gen();
//         is_valid = true;
//     }

//     return val;
// }

