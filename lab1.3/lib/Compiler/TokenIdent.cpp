#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenIdent final : Token {
    std::size_t IdentCode{};

    auto isFinished() const -> bool override {
        return str.size() % 2 == 1 && canBePrefix();
    }

    auto canBePrefix() const -> bool override {
        for (const auto& chr : str) {
            if (!isalpha(chr)) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace Compiler
