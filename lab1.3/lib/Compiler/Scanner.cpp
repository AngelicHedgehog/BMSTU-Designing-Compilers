#pragma once

#include <istream>
#include <string>
#include <vector>

#include "CompilerAbstract.cpp"
#include "Token.cpp"

namespace Compiler {

class Scanner {
public:
    Scanner(std::istream& stream, CompilerAbstract& compiler)
    : _stream(stream),
      _compiler(compiler) {};

    auto nextToken() -> const Token& {
        _compiler.Messages;
        return TokenIdent{};
    };

    std::string Program;
    std::vector<Fragment> Comments;

private:
    std::istream& _stream;
    CompilerAbstract& _compiler;
};

}  // namespace Compiler
