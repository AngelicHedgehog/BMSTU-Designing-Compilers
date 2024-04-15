#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <string>
#include <cassert>


auto errorSym(char errorSym) -> void
{
    std::cerr << "Error: unrecognized symbol '" << errorSym << "'.\n";
}

auto errorEarlyFinish() -> void
{
    std::cerr << "Error: text parsing was not completed.\n";
}

auto errorUnrecognizedTail() -> void
{
    std::cerr << "Error: unreconized tail of text.\n";
}

auto readNewChar(std::istream& inputChain) -> char
{
    char alpha;
    do
    {
        alpha = inputChain.get();
        if (alpha == EOF)
        {
            return '$';
        }
    } while ((isspace(alpha) && alpha != '\n'));
    return alpha;
}

auto TopDownParse(
    const std::unordered_set<std::string>& nonTerminals,
    const std::unordered_set<char>& terminals,
    const std::string& startTerm,
    const std::unordered_map<char, std::unordered_map<std::string, std::vector<std::string>>>& predictTable,
    std::istream& inputChain
) -> std::vector<std::pair<std::string, std::vector<std::string>>>
{
    assert(terminals.contains('$'));
    assert(
        nonTerminals.contains(startTerm) ||
        (
            startTerm.size() == 1 &&
            terminals.contains(startTerm.at(0))
        )
    );
    assert(predictTable.size() == terminals.size());
    for (const auto& terminal : terminals)
    {
        assert(predictTable.contains(terminal));
        assert(predictTable.at(terminal).size() == nonTerminals.size());
        for (const auto& nonTerminal : nonTerminals)
        {
            assert(predictTable.at(terminal).contains(nonTerminal));
        }
    }
    for (const auto& [_, rules] : predictTable)
    {
        for (const auto& [_, rule] : rules)
        {
            for (const auto& term : rule)
            {
                assert(
                    (
                        term.size() == 1 &&
                        terminals.contains(term.at(0))
                    ) ||
                    nonTerminals.contains(term) ||
                    term == "ERROR" ||
                    term == "ε"
                );
            }
        }
    }

    // inputChain.unsetf(std::ios_base::skipws);

    std::vector<std::pair<std::string, std::vector<std::string>>> result{};
    std::stack<std::string> magazine{};
    magazine.push("$");
    magazine.push(startTerm);
    char alpha = readNewChar(inputChain);
    std::string topSym;

    do
    {
        topSym = magazine.top();
        while (topSym == "ε")
        {
            magazine.pop();
            topSym = magazine.top();
        }
        if (topSym.size() == 1 && terminals.contains(topSym.at(0)))
        {
            if (topSym.at(0) == alpha)
            {
                magazine.pop();
            }
            else if (topSym == "$")
            {
                errorUnrecognizedTail();
                break;
            }
            else if (alpha == '$')
            {
                std::cout << topSym << '\n';
                errorEarlyFinish();
                break;
            }
            else
            {
                errorSym(alpha);
            }
            alpha = readNewChar(inputChain);
        }
        else if (predictTable.contains(alpha) && (
                 predictTable.at(alpha).at(topSym).size() != 1 ||
                 predictTable.at(alpha).at(topSym).at(0) != "ERROR"))
        {
            magazine.pop();
            const auto& rule = predictTable.at(alpha).at(topSym);
            for (auto it = rule.rbegin(); it != rule.rend(); ++it)
            {
                magazine.push(*it);
            }
            result.push_back({topSym, rule});
        }
        else if (alpha == '$')
        {
            errorEarlyFinish();
            break;
        }
        else
        {
            errorSym(alpha);
            alpha = readNewChar(inputChain);
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
        "Altrule",
        "Term",
        "Nterm",
        "Nterm'",
        "Comment",
        "Comment'"
    };
    std::unordered_set<char> terminals{};
    std::string startTerm = "Rules";
    std::unordered_map<char,
        std::unordered_map<std::string, std::vector<std::string>>>
        predictTable{};
    std::ifstream inputChain{"example.txt"};
    
    terminals.insert('\'');
    predictTable.insert({'\'', {
        {"Rules", {"Comment", "Rules"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"Term", "Altrule"}},
        {"Term", {"'"}},
        {"Nterm", {"ERROR"}},
        {"Nterm'", {"'"}},
        {"Comment", {"'", "Comment'", "\n"}},
        {"Comment'", {"'", "Comment'"}}}});
    terminals.insert('\n');
    predictTable.insert({'\n', {
        {"Rules", {"ERROR"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"Term", "Altrule"}},
        {"Term", {"\n"}},
        {"Nterm", {"ERROR"}},
        {"Nterm'", {"\n"}},
        {"Comment", {"ERROR"}},
        {"Comment'", {"ε"}}}});
    for (auto sym : "+-*/()")
    {
        if (sym == '\0')
        {
            continue;
        }
        terminals.insert(sym);
        predictTable.insert({sym, {
            {"Rules", {"ERROR"}},
            {"Rule", {"ERROR"}},
            {"Altrules", {"ERROR"}},
            {"Altrule", {"Term", "Altrule"}},
            {"Term", {{sym}}},
            {"Nterm", {"ERROR"}},
            {"Nterm'", {{sym}, "Nterm'"}},
            {"Comment", {"ERROR"}},
            {"Comment'", {{sym}, "Comment'"}}}});
    }
    for (auto sym : "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\
абвгдежзийклмнопрстуфхцчшщъыьэюяАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ")
    {
        if (sym == '\0')
        {
            continue;
        }
        terminals.insert(sym);
        predictTable.insert({sym, {
            {"Rules", {"ERROR"}},
            {"Rule", {"ERROR"}},
            {"Altrules", {"ERROR"}},
            {"Altrule", {"Nterm", "Altrule"}},
            {"Term", {"ERROR"}},
            {"Nterm", {{sym}, "Nterm'"}},
            {"Nterm'", {{sym}, "Nterm'"}},
            {"Comment", {"ERROR"}},
            {"Comment'", {{sym}, "Comment'"}}}});
    }
    terminals.insert('<');
    predictTable.insert({'<', {
        {"Rules", {"Rule", "Rules"}},
        {"Rule", {"<", "Nterm", "<", "Altrule", ">", "Altrules", ">", "\n"}},
        {"Altrules", {"<", "Altrule", ">", "Altrules"}},
        {"Altrule", {"ERROR"}},
        {"Term", {"ERROR"}},
        {"Nterm", {"ε"}},
        {"Nterm'", {"ε"}},
        {"Comment", {"ERROR"}},
        {"Comment'", {"<", "Comment'"}}}});

    terminals.insert('>');
    predictTable.insert({'>', {
        {"Rules", {"ERROR"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ε"}},
        {"Altrule", {"ε"}},
        {"Term", {"ERROR"}},
        {"Nterm", {"ε"}},
        {"Nterm'", {"ε"}},
        {"Comment", {"ERROR"}},
        {"Comment'", {">", "Comment'"}}}});

    terminals.insert('$');
    predictTable.insert({'$', {
        {"Rules", {"ε"}},
        {"Rule", {"ERROR"}},
        {"Altrules", {"ERROR"}},
        {"Altrule", {"ERROR"}},
        {"Term", {"ERROR"}},
        {"Nterm", {"ERROR"}},
        {"Nterm'", {"ERROR"}},
        {"Comment", {"ERROR"}},
        {"Comment'", {"ε"}}}});

    auto res = TopDownParse(nonTerminals, terminals, startTerm, predictTable, inputChain);

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
        for (const auto& nterm : rule)
        {
            if (nonTerminals.contains(nterm))
            {
                ++countNonTerms;
            }
        }
        if (countNonTerms != 0)
        {
            magazine.push(countNonTerms);
        }
    }

    inputChain.close();

    return 0;
}