#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenNterm final : Token {
    auto isFinished() const -> bool override {
        if (str.size() == 0 || !isalpha(str.at(0))) {
            return false;
        }
        for (char c : str) {
            if (isspace(c) || c == '<' || c == '>') {
                return false;
            }
        }
        return true;
    }

    auto canBePrefix() const -> bool override {
        if (str.size() != 0 && !isalpha(str.at(0))) {
            return false;
        }
        
        for (char c : str) {
            if (isspace(c) || c == '<' || c == '>') {
                return false;
            }
        }
        return true;
    }
};

}  // namespace Compiler
