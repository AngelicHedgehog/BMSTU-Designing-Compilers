#pragma once

#include <istream>
#include <string>
#include <vector>
#include <memory>

#include "CompilerAbstract.cpp"
#include "TokenIdent.cpp"
#include "TokenKeyword.cpp"
#include "ProgramaIterator.cpp"

namespace Compiler {

class Scanner {
public:
    Scanner(std::istream& stream, CompilerAbstract& compiler)
    : _programa(Program, stream),
      _compiler(compiler) {};

    auto nextToken() -> std::unique_ptr<Token> {
        // пропуск последовательности пробельных символов
        while (!_programa.eof() && isspace(_programa.cur())) {
            _programa.next();
        }
        if (_programa.eof()) {
            return nullptr;
        }
        // случай обнаружения знака комментария
        if (_programa.pos().Pos == 1 && _programa.cur() == '*') {
            Position commentStarts = _programa.pos();

            while (!_programa.eof() && _programa.cur() != '\n') {
                _programa.next();
            }
            if (!_programa.eof()) {
                _programa.next();
            }

            Comments.push_back({commentStarts, _programa.pos()});
            return nextToken();
        }

        // ищем максимально длинный префикс программы, описывающий некоторый токен
        Token finishToken{};
        finishToken.Coords.Strarting = finishToken.Coords.Ending = _programa.pos();
        ++finishToken.Coords.Ending.Index;

        std::vector<std::unique_ptr<Token>> tokens{};
        tokens.push_back(std::make_unique<TokenIdent>());
        tokens.back()->Tag = Token::DomainTag::IDENT;
        tokens.push_back(std::make_unique<TokenKeyword>());
        tokens.back()->Tag = Token::DomainTag::KEYWORD;
        for (auto& token : tokens) {
            token->Coords.Strarting = token->Coords.Ending = _programa.pos();
        }

        while (!_programa.eof() && !tokens.empty()) {

            // сохранение возможных вариантов
            for (const auto& token : tokens) {
                if (token->isFinished()) {
                    finishToken = *token.get();
                    finishToken.Coords.Ending = _programa.pos();
                }
            }

            // устранение невалидных далее вариантов
            for (
                auto it = std::crbegin(tokens);
                it != std::crend(tokens);
                ++it
            ) { 
                if (!(*it)->canBePrefix()) {
                    tokens.erase((it + 1).base());
                }
            }

            // довавление всем токенам следующего символа программы
            for (auto& token : tokens) {
                token->str += _programa.cur();
            }
            _programa.next();
        }

        // возвращение из лукапа за последний валидный токен
        _programa.prev(_programa.pos().Index - finishToken.Coords.Ending.Index);

        // вычисление внутренних аттрибутов
        switch (finishToken.Tag) {
        case Token::DomainTag::NIL: {
            auto returnToken = nextToken();

            // группировка последовательных сообщений об ошибках
            auto errorPos = finishToken.Coords.Strarting;
            auto& messages = _compiler.Messages.GetSorted();
            if (
                messages.size() != 0 &&
                errorPos.Index == std::crbegin(messages)->Coord.Index - 1
            ) {
                messages.erase(--std::cend(messages));
            }
            _compiler.Messages.AddError(
                errorPos,
                "token didnt recognized"
            );

            return returnToken;
        
        } case Token::DomainTag::IDENT: {
            auto ident = std::make_unique<TokenIdent>();
            ident->Coords = finishToken.Coords;
            ident->Tag = finishToken.Tag;
            ident->str = finishToken.str;

            // проверка на уникальность идентификатора
            if (_compiler.Names.Contains(ident->str)) {
                _compiler.Messages.AddWarning(
                    ident->Coords.Strarting,
                    "identificator alredy declared"
                );
            }
            ident->IdentCode = _compiler.Names.AddName(ident->str);
            return ident;
        
        } case Token::DomainTag::KEYWORD: {
            auto keyword = std::make_unique<TokenKeyword>();
            keyword->Coords = finishToken.Coords;
            keyword->Tag = finishToken.Tag;
            keyword->str = finishToken.str;

            if (keyword->str == "with") {
                keyword->Keyword = TokenKeyword::WITH;
            } else if (keyword->str == "end") {
                keyword->Keyword = TokenKeyword::END;
            } else if (keyword->str == "**") {
                keyword->Keyword = TokenKeyword::STAR_PAIR;
            }
            return keyword;

        } default: {
            return nullptr;
        }
        }
    };

    std::string Program;
    std::vector<Fragment> Comments;

private:
    ProgramaIterator _programa;
    CompilerAbstract& _compiler;
};

}  // namespace Compiler
