#pragma once

#include <iostream>

#include "CompilerAbstract.cpp"
#include "Scanner.cpp"

namespace Compiler {

class Compiler final : public CompilerAbstract {
public:
    auto GetScanner(std::string_view string) -> Scanner override {
        return {string, *this};
    }
};

}  // namespace Compiler
