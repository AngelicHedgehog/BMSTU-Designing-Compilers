%option reentrant noyywrap bison-bridge bison-locations caseless
%option extra-type="struct Extra *"

/* Подавление предупреждений для -Wall */
%option noinput nounput

%{

#include <stdio.h>
#include <stdlib.h>
#include "lexer.l.h"
#include "parser.h"  /* файл генерируется Bison’ом */

#define YY_USER_ACTION \
  { \
    int i; \
    struct Extra *extra = yyextra; \
    if (! extra->continued ) { \
      yylloc->first_line = extra->cur_line; \
      yylloc->first_column = extra->cur_column; \
    } \
    extra->continued = false; \
    for (i = 0; i < yyleng; ++i) { \
      if (yytext[i] == '\n') { \
        extra->cur_line += 1; \
        extra->cur_column = 1; \
      } else { \
        extra->cur_column += 1; \
      } \
    } \
    yylloc->last_line = extra->cur_line; \
    yylloc->last_column = extra->cur_column; \
  }

void yyerror(YYLTYPE *loc, yyscan_t scanner, const char *message) {
    printf("Error (%d,%d): %s\n", loc->first_line, loc->first_column, message);
}

%}

%%

\(\*([^\*]|\*[^\)])*\*\)|\{[^\}]*\}  {
    asprintf(&yylval->comment, "%s", yytext);
    return COMMENT;
}
\n  return NEW_LINE;

[\r\t ]+

\+      return '+';
\-      return '-';
\(      return LEFT_PAREN;
\)      return RIGHT_PAREN;
\.      return DOT;
\,      return COMMA;
\^      return CARET;
\;      return SEMICOLON;
\:      return COLON;
\=      return ASSIGN;

INTEGER     return KW_INTEGER;
BOOLEAN     return KW_BOOLEAN;
REAL        return KW_REAL;
CHAR        return KW_CHAR;
TEXT        return KW_TEXT;
PACKED      return KW_PACKED;
ARRAY       return KW_ARRAY;
OF          return KW_OF;
FILE        return KW_FILE;
SET         return KW_SET;
RECORD      return KW_RECORD;
END         return KW_END;
CASE        return KW_CASE;
CONST       return KW_CONST;
TYPE        return KW_TYPE;

[a-zA-Z][a-zA-Z0-9]*  {
    asprintf(&yylval->identifier, "%s", yytext);
    return IDENTIFIER;
}

[0-9]+(\.[0-9]+)?(E[\+\-]?[0-9]+)?  {
    yylval->unsigned_number = atof(yytext);
    return UNSINGNED_NUMBER;
}

'[^\']+'  {
    asprintf(&yylval->char_sequence, "%s", yytext);
    return CHAR_SEQUENCE;
}

%%

void init_scanner(FILE *input, yyscan_t *scanner, struct Extra *extra) {
    extra->continued = false;
    extra->cur_line = 1;
    extra->cur_column = 1;

    yylex_init(scanner);
    yylex_init_extra(extra, scanner);
    yyset_in(input, *scanner);
}

void destroy_scanner(yyscan_t scanner) {
    yylex_destroy(scanner);
}
