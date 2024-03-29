#pragma once

#include <string>

#include "Token.cpp"

namespace Compiler {

struct TokenIdent final : Token {
    std::size_t IdentCode{};

    auto isFinished() const -> bool override {
        return str.size() > 0 && canBePrefix();
    }

    auto canBePrefix() const -> bool override {
        if (str.size() > 0 && !isalpha(str.at(0))) {
            return false;
        }
        for (char chr : str) {
            if (!isalpha(chr) && !isdigit(chr)) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace Compiler
