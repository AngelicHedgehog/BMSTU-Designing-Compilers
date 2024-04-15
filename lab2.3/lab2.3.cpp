#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <string>
#include <cassert>

#include "lib/Compiler/Compiler.cpp"


enum class TableLexemDomen
{
    TERM,
    NTERM,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END_OF_FILE,
};

struct TabelLexem
{
    std::string str;
    std::string domen;
    Compiler::Fragment coord;
};

auto errorLexem(Compiler::MessageList& messageList, const TabelLexem& errorLex, std::string expected) -> void
{
    messageList.AddError(errorLex.coord.Strarting,
        "Unexpected lexem '" + errorLex.str + "', expected '" + expected + "'.");
}

auto errorEarlyFinish(Compiler::MessageList& messageList, const TabelLexem& errorLex) -> void
{
    messageList.AddError(errorLex.coord.Strarting,
        "Text parsing was not completed.");
}

auto errorUnrecognizedTail(Compiler::MessageList& messageList, const TabelLexem& errorLex) -> void
{
    messageList.AddError(errorLex.coord.Strarting,
        "Unreconized tail of text.");
}

auto readNewLexem(Compiler::Scanner& scanner) -> TabelLexem
{
    auto nextLexem = scanner.nextToken();
    if (!nextLexem || nextLexem->Tag == Compiler::Token::DomainTag::NIL)
    {
        return {"$", "$", nextLexem ? nextLexem->Coords.Strarting : Compiler::Position{0, 0}};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::TERM)
    {
        return {nextLexem->str, "Term", nextLexem->Coords.Strarting};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::NTERM)
    {
        return {nextLexem->str, "Nterm", nextLexem->Coords.Strarting};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::KEYWORD)
    {
        if (nextLexem->str == "<")
        {
            return {nextLexem->str, "<", nextLexem->Coords.Strarting};
        }
        else
        {
            return {nextLexem->str, ">", nextLexem->Coords.Strarting};
        }        
    }
    return {"$", "$"};
}

auto TopDownParse(
    const std::unordered_set<std::string>& nonTerminals,
    const std::string& startTerm,
    const std::unordered_map<std::string,
                             std::unordered_map<std::string, std::vector<std::string>>>&
                            predictTable,
    Compiler::Scanner& scanner,
    Compiler::MessageList& messageList
) -> std::vector<std::pair<std::string, std::vector<std::string>>>
{

    std::vector<std::pair<std::string, std::vector<std::string>>> result{};
    std::stack<std::string> magazine{};
    magazine.push("$");
    magazine.push(startTerm);
    auto alpha = readNewLexem(scanner);
    std::string topSym;

    do
    {
        topSym = magazine.top();
        while (topSym == "ε")
        {
            magazine.pop();
            topSym = magazine.top();
            result.push_back({"ε", {"\"ε\""}});
        }
        if (!nonTerminals.contains(topSym))
        {
            if (topSym == alpha.domen)
            {
                magazine.pop();
                result.push_back({alpha.domen, {"\"" + alpha.str + "\""}});
            }
            else if (topSym == "$")
            {
                errorUnrecognizedTail(messageList, alpha);
                break;
            }
            else if (alpha.domen == "$")
            {
                std::cout << topSym << '\n';
                errorEarlyFinish(messageList, alpha);
                break;
            }
            else
            {
                errorLexem(messageList, alpha, topSym);
            }
            alpha = readNewLexem(scanner);
        }
        else if (predictTable.at(alpha.domen).at(topSym).size() != 1 ||
                 predictTable.at(alpha.domen).at(topSym).at(0) != "ERROR")
        {
            magazine.pop();
            const auto& rule = predictTable.at(alpha.domen).at(topSym);
            for (auto it = rule.rbegin(); it != rule.rend(); ++it)
            {
                magazine.push(*it);
            }
            result.push_back({topSym, rule});
        }
        else if (alpha.domen == "$")
        {
            std::cout << topSym << '\n';
            errorEarlyFinish(messageList, alpha);
            break;
        }
        else
        {
            errorLexem(messageList, alpha, topSym);
            alpha = readNewLexem(scanner);
        }
    }
    while (topSym != "$");

    return result;
}

int main()
{
    std::unordered_set<std::string> nonTerminals{
        "Rules",
        "Rule",
        "Altrules",
        "Altrule"
    };
    std::string startTerm = "Rules";
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::vector<std::string>>>
        predictTable{};
    std::ifstream inputChain{"example.txt"};
    
    
    predictTable.insert({"Nterm", {
        {"Rules", {"ERROR"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"Nterm", "Altrule"}}}});

    
    predictTable.insert({"Term", {
        {"Rules", {"ERROR"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"Term", "Altrule"}}}});

    predictTable.insert({"<", {
        {"Rules", {"Rule", "Rules"}},
        {"Rule", {"<", "Nterm", "<", "Altrule", ">", "Altrules", ">"}},
        {"Altrules", {"<", "Altrule", ">", "Altrules"}},
        {"Altrule", {"ERROR"}}}});

    predictTable.insert({">", {
        {"Rules", {"ERROR"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ε"}},
        {"Altrule", {"ε"}}}});

    predictTable.insert({"$", {
        {"Rules", {"ε"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"ERROR"}}}});

    Compiler::Compiler compiler{};
    auto scanner = compiler.GetScanner(inputChain);

    auto res = TopDownParse(nonTerminals, startTerm, predictTable, scanner, compiler.Messages);

    std::stack<std::size_t> magazine{};


    auto printMag = [&]()
    {
        std::vector<std::size_t> temp{};
        while (!magazine.empty())
        {
            temp.push_back(magazine.top());
            magazine.pop();
        }
        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
        {
            std::cout << *it << ' ';
            magazine.push(*it);
        }
        std::cout << '\n';
    };
    for (const auto& [nonTerm, rule] : res)
    {
        while (!magazine.empty() && magazine.top() == 0)
        {
            magazine.pop();
        }
        // printMag();
        for (std::size_t i = 0; i != magazine.size(); ++i)
        {
            std::cout << "  ";
        }
        std::cout << nonTerm << " -> ";
        for (const auto& ruleTerm : rule)
        {
            std::cout << ruleTerm << ' ';
        }
        std::cout << "\n";

        if (!magazine.empty())
        {
            std::size_t lastTermCount = magazine.top();
            magazine.pop();
            magazine.push(lastTermCount - 1);
        }

        std::size_t countNonTerms = 0;
        if (nonTerminals.contains(nonTerm))
        {
            countNonTerms = rule.size();
        }
        if (countNonTerms != 0)
        {
            magazine.push(countNonTerms);
        }
    }

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

    inputChain.close();

    return 0;
}