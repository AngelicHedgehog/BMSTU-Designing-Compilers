#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenInteger final : Token {
    std::size_t Value;

    auto isFinished() const -> bool override {
        return canBePrefix() && str.size() > 0;
    }

    auto canBePrefix() const -> bool override {
        for (char digit : str) {
            if (!isdigit(digit)) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace Compiler
