#include <iostream>
#include <fstream>

#include "lib/Compiler/Compiler.cpp"

auto main() -> int {
    std::ifstream fileStream{"prog.txt"};

    Compiler::Compiler compiler{};
    auto scanner = compiler.GetScanner(fileStream);

    

    fileStream.close();
    return 0;
}
