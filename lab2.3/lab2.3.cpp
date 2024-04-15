#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <string>
#include <cassert>

#include "lib/Compiler/Compiler.cpp"


struct TabelLexemUnique
{
    std::string nonTerm;
    std::string domen;
    std::unique_ptr<Compiler::Token> token;
};

struct TabelLexemShared
{
    std::string nonTerm;
    std::string domen;
    std::shared_ptr<Compiler::Token> token;

    auto make_unique() -> TabelLexemUnique
    {
        return {nonTerm, domen, std::make_unique<Compiler::Token>(*token)};
    }
};

auto errorLexem(Compiler::MessageList& messageList,
    const TabelLexemShared& errorLex, std::string expected) -> void
{
    messageList.AddError(errorLex.token->Coords.Strarting,
        "Unexpected lexem '" + errorLex.token->str + "', expected '" + expected + "'.");
}

auto errorEarlyFinish(Compiler::MessageList& messageList,
    const TabelLexemShared& errorLex) -> void
{
    messageList.AddError(errorLex.token->Coords.Strarting,
        "Text parsing was not completed.");
}

auto errorUnrecognizedTail(Compiler::MessageList& messageList,
    const TabelLexemShared& errorLex) -> void
{
    messageList.AddError(errorLex.token->Coords.Strarting,
        "Unreconized tail of text.");
}

auto readNewLexem(Compiler::Scanner& scanner) -> TabelLexemShared
{
    auto nextToken = scanner.nextToken();
    auto nextLexem = nextToken ? std::make_shared<Compiler::Token>(*nextToken) : nullptr;
    if (!nextLexem)
    {
        return {"", "$", std::make_shared<Compiler::Token>()};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::NIL)
    {
        return {"", "$", nextLexem};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::TERM)
    {
        return {"", "Term", nextLexem};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::NTERM)
    {
        return {"", "Nterm", nextLexem};
    }
    if (nextLexem->Tag == Compiler::Token::DomainTag::KEYWORD)
    {
        if (nextLexem->str == "<")
        {
            return {"", "<", nextLexem};
        }
        else
        {
            return {"", ">", nextLexem};
        }        
    }
    return {"", "$", std::make_shared<Compiler::Token>()};
}

auto TopDownParse(
    const std::unordered_set<std::string>& nonTerminals,
    const std::string& startTerm,
    const std::unordered_map<std::string,
                             std::unordered_map<std::string, std::vector<std::string>>>&
                            predictTable,
    Compiler::Scanner& scanner,
    Compiler::MessageList& messageList
) -> std::vector<std::pair<TabelLexemUnique, std::vector<std::string>>>
{

    std::vector<std::pair<TabelLexemUnique, std::vector<std::string>>> result{};
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
            result.push_back({TabelLexemUnique{"ε", "ε", std::make_unique<Compiler::Token>()},
                std::vector<std::string>{"ε"}});
        }
        alpha.nonTerm = topSym;
        if (!nonTerminals.contains(topSym))
        {
            if (topSym == alpha.domen)
            {
                magazine.pop();
                result.push_back({alpha.make_unique(),
                    std::vector<std::string>{alpha.token->str}});
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
            result.push_back({alpha.make_unique(), rule});
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

    for (const auto& [nonTerm, rule] : res)
    {
        while (!magazine.empty() && magazine.top() == 0)
        {
            magazine.pop();
        }
        for (std::size_t i = 0; i != magazine.size(); ++i)
        {
            std::cout << "  ";
        }
        std::cout << nonTerm.nonTerm << " -> ";
        if (!nonTerminals.contains(nonTerm.nonTerm))
        {
            std::cout << "\'" << nonTerm.domen << "\' ";
            if (nonTerm.nonTerm != "$" && nonTerm.nonTerm != "ε")
            {
                std::cout << nonTerm.token->Coords.Strarting <<
                '-' <<
                nonTerm.token->Coords.Ending;
            }
            if (nonTerm.token->Tag != nonTerm.token->KEYWORD &&
                nonTerm.token->Tag != nonTerm.token->NIL)
            {
                std::cout << " \"" << nonTerm.token->str << "\"";
            }
        }
        else
        {
            for (const auto& ruleTerm : rule)
            {
                std::cout << ruleTerm << ' ';
            }
        }
        std::cout << "\n";

        if (!magazine.empty())
        {
            std::size_t lastTermCount = magazine.top();
            magazine.pop();
            magazine.push(lastTermCount - 1);
        }

        std::size_t countNonTerms = 0;
        if (nonTerminals.contains(nonTerm.nonTerm))
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