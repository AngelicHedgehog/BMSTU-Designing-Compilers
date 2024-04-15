#pragma once

#include <string>
#include <vector>

namespace Compiler {

class NameDictionary {
public:
    auto AddName(const std::string& string) -> std::size_t {
        _dict.push_back(string);
        return _dict.size() - 1;
    };

    auto Contains(const std::string& string) const -> bool {
        for (const auto& el : _dict) {
            if (el == string) {
                return true;
            }
        }
        return false;
    };

    auto GetName(std::size_t code) const -> std::string {
        return _dict.at(code);
    }
private:
    std::vector<std::string> _dict;
};

}  // namespace Compiler
