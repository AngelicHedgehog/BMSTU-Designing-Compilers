#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenKeyword final : Token {
    enum DomainKeyword {
        WHERE,
        ARROW,
        DOUBLE_ARROW,
    };
    DomainKeyword Keyword;

    auto isFinished() const -> bool override {
        return str == "where" || str == "->" || str == "=>";
    }

    auto canBePrefix() const -> bool override {
        switch (str.size()) {
        case 0:
            return true;
        case 1:
            return str == "w" || str == "-" || str == "=";
        case 2:
            return str == "wh" || str == "->" || str == "=>";
        case 3:
            return str == "whe";
        case 4:
            return str == "wher";
        case 5:
            return str == "where";
        default:
            return false;
        }
    }
};

}  // namespace Compiler
