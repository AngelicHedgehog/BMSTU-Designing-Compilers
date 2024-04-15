#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenKeyword final : Token {
    enum DomainKeyword {
        OPEN_BRACKET,
        CLOSE_BRACKET,
    };
    DomainKeyword Keyword;

    auto isFinished() const -> bool override {
        return str == "<" || str == ">";
    }

    auto canBePrefix() const -> bool override {
        switch (str.size()) {
        case 0:
            return true;
        case 1:
            return str == "<" || str == ">";
        default:
            return false;
        }
    }
};

}  // namespace Compiler
