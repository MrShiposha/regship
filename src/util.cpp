#include "util.hpp"

size_t combine_hashes(size_t hash_a, size_t hash_b) {
    size_t hash = hash_a ^ (hash_b + 0x9e3779b9 + (hash_a << 6) + (hash_a >> 2));
    return hash;
}
