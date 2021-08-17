#include "regsymstream.hpp"
#include "util.hpp"

RegSymStream::RegSymStream(const std::string_view str)
    : str(str)
{}

RegSymStream &RegSymStream::operator>>(RegSymbol &out) {
    auto symbol = read_reg_symbol(str);
    str.remove_prefix(symbol.size());

    out = symbol;
    return *this;
}

RegSymStream::operator bool() const {
    return !is_empty();
}

bool RegSymStream::operator!() const {
    return is_empty();
}

bool RegSymStream::is_empty() const {
    return str.empty();
}
