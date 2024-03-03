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

        while (isspace(_stream.peek()) && !_stream.eof()) {
            if (_stream.get() == '\n') {
                ++actualPos.Line;
                actualPos.Pos = 1;

                // комментарий
                if (_stream.peek() == '*') {
                    Position commentStarts = actualPos;

                    while (_stream.get() != '\n' || _stream.eof()) {
                        ++actualPos.Pos;
                    }
                    ++actualPos.Line;
                    actualPos.Pos = 1;

                    Comments.push_back({commentStarts, actualPos});
                }
            } else {
                ++actualPos.Pos;
            }
        }
        if (_stream.eof()) {
            return {Token::DomainTag::NIL};
        }

        

        return TokenIdent{};
    };

    std::string Program;
    std::vector<Fragment> Comments;

private:
    std::istream& _stream;
    CompilerAbstract& _compiler;

    Position actualPos{1, 1, 0};
};

}  // namespace Compiler
