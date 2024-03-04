#include <iostream>
#include <fstream>
#include <memory>

#include "lib/Compiler/Compiler.cpp"

auto main() -> int {
    std::ifstream fileStream{"prog.txt"};

    Compiler::Compiler compiler{};
    auto scanner = compiler.GetScanner(fileStream);

    std::cout << "TOKENS:\n";
    std::unique_ptr<Compiler::Token> nextToken = scanner.nextToken();
    while (nextToken != nullptr) {
        std::cout << '\t';
        switch (nextToken->Tag) {
        case Compiler::Token::IDENT:
            std::cout << "IDENT";
            break;
        
        case Compiler::Token::KEYWORD:
            std::cout << "KEYWORD";
            break;
        }
        std::cout
            << ' '
            << nextToken->Coords.Strarting
            << '-'
            << nextToken->Coords.Ending
            << ": "
            << nextToken->str
            << '\n';

        nextToken = scanner.nextToken();
    };

    std::cout << "COMMENTS:\n";
    for (const auto& comment : scanner.Comments) {
        std::cout
            << '\t'
            << comment.Strarting
            << '-'
            << comment.Ending
            << '\n';
    }

    std::cout << "MESSAGES:\n";
    for (const auto& message : compiler.Messages.GetSorted()) {
        std::cout
            << '\t'
            << (message.IsError ? "ERROR " : "WRANING ")
            << message.Coord
            << ": "
            << message.Text
            << '\n';
    }

    std::cout << "PROGRAM:\n" << scanner.Program << '\n';

    fileStream.close();
    return 0;
}
