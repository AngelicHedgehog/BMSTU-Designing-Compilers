#pragma once

#include "MessageList.cpp"
#include "NameDictionary.cpp"

namespace Compiler {

class Scanner;

class CompilerAbstract {
public:
    virtual auto GetScanner(std::istream& stream) -> Scanner = 0;

    MessageList Messages;
    NameDictionary Names;
};

}  // namespace Compiler
