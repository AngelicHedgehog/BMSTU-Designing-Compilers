% Лабораторная работа № 2.4 «Множества FIRST для РБНФ»
% <лабораторная ещё не сдана>
% Сергей Виленский, ИУ9-62Б

# Цель работы
Целью данной работы является изучение алгоритмов построения парсеров методом рекурсивного спуска.

# Индивидуальный вариант
Объявления типов и констант в Паскале:

В `record`’е точка с запятой разделяет поля и после `case` дополнительный end не ставится. См. https://bernd-oppolzer.de/PascalReport.pdf, третья с конца страница.

```
Type
  Coords = Record x, y: INTEGER end;
Const
  MaxPoints = 100;
type
  CoordsVector = array 1..MaxPoints of Coords;

(* графический и текстовый дисплеи *)
const
  Heigh = 480;
  Width = 640;
  Lines = 24
  Columns = 80;
type
  BaseColor = (red, green, blue, highlited);
  Color = set of BaseColor;
  GraphicScreen = array 1..Heigh of array 1..Width of Color;
  TextScreen = array 1..Lines of array 1..Columns of
    record
      Symbol : CHAR;
      SymColor : Color;
      BackColor : Color
    end;

{ определения токенов }
TYPE
  Domain = (Ident, IntNumber, RealNumber);
  Token = record
    fragment : record
      start, following : record
        row, col : INTEGER
      end
    end;
    case tokType : Domain of
      Ident : (
        name : array 1..32 of CHAR
      );
      IntNumber : (
        intval : INTEGER
      );
      RealNumber : (
        realval : REAL
      )
  end;

  Year = 1900..2050;

  List = record
    value : Token;
    next : ^List
  end;
```

Ключевые слова и идентификаторы не чувствительны к регистру.

# Реализация

## Лексическая структура
Лексическая структура в порядке убывания приоритетов доменов
```
TypeIdentifier      -> INTEGER
                     | BOOLEAN
                     | REAL
                     | CHAR
                     | TEXT
ConstantIdentifier  -> Identifier
Identifier          -> Letter (Letter|Digit)*
Letter              -> [a-zA-Z]
UnsignedInteger     -> Digit+
UnsignedNumber      -> UnsignedInteger (. Digit+)? (E [+-]? UnsignedInteger)?
Digit               -> [0-9]
```

## Грамматика языка
```
# program
Program -> Block

# block
Block -> KW_CONST BlockConstSequence Block
       | KW_TYPE BlockTypeSequence Block
       | ε
BlockConstSequence -> BlockConst
                    | BlockConst BlockConstSequence
BlockConst -> IDENTIFIER '=' Constant ';'
BlockTypeSequence -> BlockType
                   | BlockType BlockTypeSequence
BlockType -> IDENTIFIER '=' Type ';'

# field list
FieldList -> IdentifierWithTypeList
           | IdentifierWithTypeSeq CaseBlock
IdentifierWithTypeList -> IdentifierWithType
                        | IdentifierWithType ';' IdentifierWithTypeList
IdentifierWithTypeSeq -> IdentifierWithType ';'
                       | IdentifierWithType ';' IdentifierWithTypeSeq
IdentifierWithType -> IdentifierList ':' Type
CaseBlock -> KW_CASE IDENTIFIER ':' TypeIdentifier KW_OF CaseVariantSequence
CaseVariantSequence -> CaseVariant
                     | CaseVariant ';' CaseVariantSequence
CaseVariant -> ConstantList ':' '(' FieldList ')'
ConstantList -> Constant
              | Constant ',' ConstantList

# type
Type -> SimpleType
      | '^' Typeidentifier
      | KW_PACKED TypeAfterPacked
      | TypeAfterPacked

TypeAfterPacked -> KW_ARRAY SimpleTypeList KW_OF Type
                 | KW_FILE KW_OF Type
                 | KW_SET KW_OF SimpleType
                 | KW_RECORD FieldList KW_END
SimpleTypeList -> SimpleType
                | SimpleType ',' SimpleTypeList

# simple type
SimpleType -> TypeIdentifier
            | '(' IdentifierList ')'
            | Constant '.' '.' Constant
TypeIdentifier -> CommonTypeIdentifier
                | IDENTIFIER
CommonTypeIdentifier -> KW_INTEGER
                      | KW_BOOLEAN
                      | KW_REAL
                      | KW_CHAR
                      | KW_TEXT
IdentifierList -> IDENTIFIER
                | IDENTIFIER ',' IdentifierList

# constant
Constant -> UnarSign ConstantIdentifier
          | ConstantIdentifier
          | UnarSign UNSIGNED_NUMBER
          | UNSIGNED_NUMBER
          | '\'' CHAR_SEQUENCE '\''
UnarSign -> '+'
          | '-'
ConstantIdentifier -> IDENTIFIER
```

## Программная реализация

```
…
```

# Тестирование

Входные данные

```
Type
  Coords = Record x, y: INTEGER end;
Const
  MaxPoints = 100;
type
  CoordsVector = array 1..MaxPoints of Coords;

const
  Heigh = 480;
  Width = 640;
  Lines = 24;
  Columns = 80;
type
  BaseColor = (red, green, blue, highlited);
  Color = set of BaseColor;
  GraphicScreen = array 1..Heigh of array 1..Width of Color;
  TextScreen = array 1..Lines of array 1..Columns of
    record
      Symbol : CHAR;
      SymColor : Color;
      BackColor : Color
    end;

(* определения токенов }
{ определения токенов *)
(* определения токенов *)
{ определения токенов }
{ определения токенов *)
(* определения токенов }
TYPE
  Domain = (Ident, IntNumber, RealNumber);
  Token = record
    fragment : record
      start, following : record
        row, col : INTEGER
      end
    end;
    case tokType : Domain of
      Ident : (
        name : array 1..32 of CHAR
      );
      IntNumber : (
        intval : INTEGER
      );
      RealNumber : (
        realval : REAL
      )
  end;

  Year = 1900..2050;

  List = record
    value : Token;
    next : ^List
  end;
```

Вывод на `stdout`

```
…
```

# Вывод
‹пишете, чему научились›