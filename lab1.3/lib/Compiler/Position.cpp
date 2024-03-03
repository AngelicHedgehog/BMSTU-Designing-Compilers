#pragma once

#include <string>

namespace Compiler {

struct Position {
    std::size_t Line;
    std::size_t Pos;
    std::size_t Index;
};

}  // namespace Compiler
