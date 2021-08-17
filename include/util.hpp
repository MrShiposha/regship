#ifndef ___REGSHIP_UTIL_HPP___
#define ___REGSHIP_UTIL_HPP___

#include <functional>
#include <string_view>

size_t combine_hashes(size_t hash_a, size_t hash_b);

std::string_view read_symbol(const std::string_view str);

#endif // ___REGSHIP_UTIL_HPP___
