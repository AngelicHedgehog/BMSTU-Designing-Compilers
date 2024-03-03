#include <string>

#include "DomainTag.cpp"
#include "Fragment.cpp"

namespace Compiler {

struct Token {
    DomainTag Tag;
    Fragment Coords;

    bool isFinished;
    bool canBePrefix;
};

struct TokenIdent : Token {
    std::string Ident;

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
