#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenKeyword final : Token {
    enum DomainKeyword {
        WITH,
        END,
        STAR_PAIR,
    };
    DomainKeyword Keyword;

    auto isFinished() const -> bool override {
        return str == "with" || str == "end" || str == "**";
    }

    auto canBePrefix() const -> bool override {
        switch (str.size()) {
        case 0:
            return true;
        case 1:
            return str == "w" || str == "e" || str == "*";
        case 2:
            return str == "wi" || str == "en" || str == "**";
        case 3:
            return str == "wit" || str == "end";
        case 4:
            return str == "with";
        default:
            return false;
        }
    }
};

}  // namespace Compiler
