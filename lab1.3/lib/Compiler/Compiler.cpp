#include <string>

#include "Scanner.cpp"
#include "MessageList.cpp"
#include "NameDictionary.cpp"

namespace Compiler {

class Compiler {
public:
    auto GetScanner(std::istream& stream) -> Scanner {
        return {stream, *this};
    }

    MessageList Messages;
    NameDictionary Names;
};

}  // namespace Compiler
