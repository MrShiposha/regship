#include <iostream>

#include "regparser.hpp"

constexpr auto EXPECTED_ARGS_NUM = 3;
constexpr auto ARGV_REGEXPR_IDX = 1;
constexpr auto ARGV_IN_STRING_IDX = 2;

int main(int argc, char **argv) try {
    if (argc != EXPECTED_ARGS_NUM) {
        std::cerr << "Usage: reg-ship <reg expr> <string to match>" << std::endl;
        return -1;
    }

    RegParser parser;
    auto matcher = parser.parse(argv[ARGV_REGEXPR_IDX]);

    std::string_view prefix;
    std::string_view match_result;

    if (matcher.is_matches(argv[ARGV_IN_STRING_IDX])) {
        prefix = "[YES]";
        match_result = "matches";
    } else {
        prefix = "[NO]";
        match_result = "DOES NOT match";
    }

    std::cout << prefix
              << " the input string "
              << match_result
              << "the regular expression"
              << std::endl;

    return 0;
} catch (const std::exception &e) {
    std::cerr << "error: " << e.what() << std::endl;
} catch (...) {
    std::cerr << "unknown error" << std::endl;
}
