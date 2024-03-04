#pragma once

#include <iostream>

#include "CompilerAbstract.cpp"
#include "Scanner.cpp"

namespace Compiler {

class Compiler final : public CompilerAbstract {
public:
    auto GetScanner(std::istream& stream) -> Scanner override {
        return {stream, *this};
    }
};

}  // namespace Compiler
