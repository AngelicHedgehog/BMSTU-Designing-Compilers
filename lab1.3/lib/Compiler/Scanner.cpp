#pragma once

#include <istream>
#include <string>
#include <vector>
#include <memory>

#include "CompilerAbstract.cpp"
#include "TokenIdent.cpp"
#include "TokenOperator.cpp"
#include "TokenKeyword.cpp"
#include "TokenInteger.cpp"
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

        // случай обнаружения однострочного комментария
        if (!_programa.eof() && _programa.cur() == '-') {
            _programa.next();
            if (_programa.cur() == '-') {
                _programa.prev();
                Position commentStarts = _programa.pos();

                while (!_programa.eof() && _programa.cur() != '\n') {
                    _programa.next();
                }
                if (!_programa.eof()) {
                    _programa.next();
                }

                Comments.push_back({commentStarts, _programa.pos()});
                return nextToken();
            } else {
                _programa.prev();
            }
        }

        // случай обнаружения многострочного комментария
        if (!_programa.eof() && _programa.cur() == '{') {
            _programa.next();
            if (_programa.cur() == '-') {
                _programa.prev();
                Position commentStarts = _programa.pos();
                _programa.next();

                while (!_programa.eof()) {
                    if (_programa.cur() == '-') {
                        _programa.next();
                        if (_programa.cur() == '}') {
                            break;
                        }
                        _programa.prev();
                    }
                    _programa.next();
                }
                if (!_programa.eof()) {
                    _programa.next();
                }

                Comments.push_back({commentStarts, _programa.pos()});
                return nextToken();
            } else {
                _programa.prev();
            }
        }

        // конец файла
        if (_programa.eof()) {
            return nullptr;
        }

        // ищем максимально длинный префикс программы, описывающий некоторый токен
        Token finishToken{};
        finishToken.Coords.Strarting = finishToken.Coords.Ending = _programa.pos();
        ++finishToken.Coords.Ending.Index;

        std::vector<std::unique_ptr<Token>> tokens{};
        tokens.push_back(std::make_unique<TokenIdent>());
        tokens.back()->Tag = Token::DomainTag::IDENT;
        tokens.push_back(std::make_unique<TokenOperator>());
        tokens.back()->Tag = Token::DomainTag::OPERATOR;
        tokens.push_back(std::make_unique<TokenKeyword>());
        tokens.back()->Tag = Token::DomainTag::KEYWORD;
        tokens.push_back(std::make_unique<TokenInteger>());
        tokens.back()->Tag = Token::DomainTag::INTEGER;
        for (auto& token : tokens) {
            token->Coords.Strarting = token->Coords.Ending = _programa.pos();
        }

        while (!tokens.empty()) {

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

            // конец файла
            if (_programa.eof()) {
                break;
            }

            // довавление всем токенам следующего символа программы
            for (auto& token : tokens) {
                token->str += _programa.cur();
            }
            _programa.next();
        }

        // возвращение из лукапа за последний валидный токен
        _programa.prev(_programa.pos().Index - finishToken.Coords.Ending.Index);

        _programa.resetProgramaBuffer();

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
        
        } case Token::DomainTag::OPERATOR: {
            auto operat = std::make_unique<TokenOperator>();
            operat->Coords = finishToken.Coords;
            operat->Tag = finishToken.Tag;
            operat->str = finishToken.str;
            return operat;

        } case Token::DomainTag::KEYWORD: {
            auto keyword = std::make_unique<TokenKeyword>();
            keyword->Coords = finishToken.Coords;
            keyword->Tag = finishToken.Tag;
            keyword->str = finishToken.str;

            if (keyword->str == "where") {
                keyword->Keyword = TokenKeyword::WHERE;
            } else if (keyword->str == "->") {
                keyword->Keyword = TokenKeyword::ARROW;
            } else if (keyword->str == "=>") {
                keyword->Keyword = TokenKeyword::DOUBLE_ARROW;
            }
            return keyword;

        } case Token::DomainTag::INTEGER: {
            auto integer = std::make_unique<TokenInteger>();
            integer->Coords = finishToken.Coords;
            integer->Tag = finishToken.Tag;
            integer->str = finishToken.str;
            integer->Value = std::stoi(integer->str);
            return integer;

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
