#include <stdexcept>

#include "util.hpp"

size_t combine_hashes(size_t hash_a, size_t hash_b) {
    size_t hash = hash_a ^ (hash_b + 0x9e3779b9 + (hash_a << 6) + (hash_a >> 2));
    return hash;
}

bool is_unicode_first_byte(unsigned char byte) {
    return (byte & 0xC0) != 0x80;
}

RegSymbol read_reg_symbol(const std::string_view str) {
    auto beg = str.begin();
    auto it = beg;
    auto end = str.end();

    if (beg == end) {
        return str;
    }

    if (!is_unicode_first_byte(*it)) {
        throw std::invalid_argument("str doesn't start with valid Unicode character");
    }

    for (++it; it != end && !is_unicode_first_byte(*it); ++it);

    auto sz = (it - beg);
    return std::string_view(str.data(), sz);
}
