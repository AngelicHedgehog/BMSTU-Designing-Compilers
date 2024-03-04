#pragma once

#include <string>
#include <unordered_map>
#include <cassert>

#include "Position.cpp"

namespace Compiler {

class ProgramaIterator {
public:
    ProgramaIterator(std::string_view programa)
    : _programa(programa) {}

    auto pos() const -> Position {
        return _position;
    }

    auto cur() const -> char {
        return _programa.at(_position.Index);
    }

    auto eof() const -> char {
        return _position.Index == _programa.size();
    }

    auto next() -> void {
        assert(!eof());

        if (cur() == '\n') {
            _linesLens.insert({_position.Line, _position.Pos});
            ++_position.Line;
            _position.Pos = 1;
        } else {
            ++_position.Pos;
        }
        ++_position.Index;
    }

    auto next(std::size_t n) -> void {
        for (std::size_t i = 0; i != n; ++i) {
            next();
        }
    }

    auto prev() -> void {
        assert(_position.Index != 0);

        --_position.Index;
        if (cur() == '\n') {
            --_position.Line;
            _position.Pos = _linesLens.at(_position.Line);
        } else {
            --_position.Pos;
        }
    }

    auto prev(std::size_t n) -> void {
        for (std::size_t i = 0; i != n; ++i) {
            prev();
        }
    }

private:
    std::unordered_map<std::size_t, std::size_t> _linesLens;
    std::string _programa;
    Position _position;
};

}  // namespace Compiler
