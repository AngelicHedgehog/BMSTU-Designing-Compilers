#include <string>

#include "Position.cpp"

namespace Compiler {

struct Message {
    bool IsError;
    std::string Text;
    Position Coord;
};

auto operator<(const Message& leftMsg, const Message& rightMsg) -> bool {
    return leftMsg.Coord.Index < rightMsg.Coord.Index;
}

}  // namespace Compiler
