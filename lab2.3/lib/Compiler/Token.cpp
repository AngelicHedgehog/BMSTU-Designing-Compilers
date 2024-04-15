#pragma once

#include <string>

#include "Fragment.cpp"

namespace Compiler {

struct Token {
    Fragment Coords;
    enum DomainTag {
        NIL,
        TERM,
        NTERM,
        KEYWORD,
    };
    DomainTag Tag = NIL;

    std::string str;

    virtual auto isFinished() const -> bool {return false;};
    virtual auto canBePrefix() const -> bool {return false;};
    virtual ~Token() {};
};

}  // namespace Compiler
