#include <string>
#include <sstream>
#include <vector>

#include "Token.cpp"

namespace Compiler {

class Compiler;

class Scanner {
public:
    Scanner(std::istream& stream, Compiler& compiler)
    : _stream(stream),
      _compiler(compiler) {};

    auto nextToken() -> const Token& {
        return TokenIdent{};
    };

    std::string Program;
    std::vector<Fragment> Comments;

private:
    std::istream& _stream;
    Compiler& _compiler;
};

}  // namespace Compiler
