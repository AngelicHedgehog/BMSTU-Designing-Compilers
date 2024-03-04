#pragma once

#include <string>
#include <set>

#include "Position.cpp"
#include "Message.cpp"

namespace Compiler {

class MessageList {
public:
    auto AddError(const Position& coord, const std::string& text) -> void {
        _addMessage(true, coord, text);
    }

    auto AddWarning(const Position& coord, const std::string& text) -> void {
        _addMessage(false, coord, text);
    }

    auto GetSorted() -> std::set<Message>& {
        return _messages;
    }

private:
    auto _addMessage(bool isError, const Position& coord, const std::string& text) -> void {
        _messages.insert({isError, text, coord});
    }

    std::set<Message> _messages{};
};

}  // namespace Compiler
