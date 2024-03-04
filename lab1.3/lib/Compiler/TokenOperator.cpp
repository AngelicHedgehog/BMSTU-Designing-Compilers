#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenOperator final : Token {

    auto isFinished() const -> bool override {
        return canBePrefix() && (
            str.size() == 1 && str.at(0) != '`' ||
            str.size() > 1 && *std::rbegin(str) == '`'
        );
    }

    auto canBePrefix() const -> bool override {
        if (str.size() == 0) {
            return true;
        }
        if (str.size() == 1) {
            char chr = str.at(0);
            return
                chr == '!'  ||
                chr == '#'  ||
                chr == '$'  ||
                chr == '%'  ||
                chr == '&'  ||
                chr == '*'  ||
                chr == '+'  ||
                chr == '.'  ||
                chr == '/'  ||
                chr == '<'  ||
                chr == '='  ||
                chr == '>'  ||
                chr == '?'  ||
                chr == '@'  ||
                chr == '\\' ||
                chr == '^'  ||
                chr == '|'  ||
                chr == '-'  ||
                chr == '~'  ||
                chr == '`';
        }
        if (str.at(0) != '`') {
            return false;
        }
        if (str.size() >= 2) {
            if (!isalpha(str.at(1))) {
                return false;
            }
        }
        for (std::size_t i = 2; i < str.size() - 1; ++i) {
            if (!isalpha(str.at(i)) && !isdigit(str.at(i))) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace Compiler
