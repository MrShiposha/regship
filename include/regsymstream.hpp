#ifndef ___REGSHIP_REGSYMSTREAM_HPP___
#define ___REGSHIP_REGSYMSTREAM_HPP___

#include <string_view>

#include "regsymbol.hpp"

class RegSymStream final {
public:
    RegSymStream() = default;
    RegSymStream(const RegSymStream &) = default;
    RegSymStream(RegSymStream &&) = default;
    RegSymStream &operator=(const RegSymStream &) = default;
    RegSymStream &operator=(RegSymStream &&) = default;

    RegSymStream(const std::string_view str);

    ~RegSymStream() = default;

    RegSymStream &operator>>(RegSymbol &);

    operator bool() const;

    bool operator!() const;

    bool is_empty() const;

private:
    std::string_view str;
};

#endif // ___REGSHIP_REGSYMSTREAM_HPP___
