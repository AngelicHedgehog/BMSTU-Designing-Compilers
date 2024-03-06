#pragma once

#include <string>
#include <iostream>

struct Position {
    std::size_t Line = 1;
    std::size_t Pos = 1;
    std::size_t Index = 0;
};

auto operator<<(std::ostream& output, const Position& position) -> std::ostream& {
    return output
        << '('
        << position.Line
        << ", "
        << position.Pos
        << ')';
}
