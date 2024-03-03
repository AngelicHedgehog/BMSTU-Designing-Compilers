#pragma once

#include <string>

#include "Fragment.cpp"

namespace Compiler {

struct Token {
    enum class DomainTag {
        NIL,
        IDENT,
        KEYWORD,
    };

    DomainTag Tag;
    Fragment Coords;

    bool isFinished;
    bool canBePrefix;
};

struct TokenIdent : Token {
    std::size_t IdentCode;

};

struct TokenKeyword : Token {
    enum DomainKeyword {
        WITH,
        END,
        STAR_PAIR,
    };

    DomainKeyword Keyword;
};

}  // namespace Compiler
