#ifndef ___REGSHIP_UTIL_HPP___
#define ___REGSHIP_UTIL_HPP___

#include <functional>
#include <string_view>

#include "regsymbol.hpp"

size_t combine_hashes(size_t hash_a, size_t hash_b);

RegSymbol read_reg_symbol(const std::string_view str);

#endif // ___REGSHIP_UTIL_HPP___
