%option noyywrap bison-bridge bison-locations

%{

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

enum TAGS {
    TAG_IDENT = 1,
    TAG_NUMBER,
    TAG_CHAR,
    TAG_LPAREN,
    TAG_RPAREN,
    TAG_PLUS,
    TAG_MINUS,
    TAG_MULTIPLY,
    TAG_DIVIDE,
};

char *tag_names[] = {
    "END_OF_PROGRAM", "IDENT" , "NUMBER",
    "CHAR", "LPAREN", "RPAREN", "PLUS",
    "MINUS" , "MULTIPLY", "DIVIDE"
};

struct Position {
    int line, pos, index;
};

void print_pos(struct Position *p) {
    printf("(%d,%d)", p->line, p->pos);
}

struct Fragment {
    struct Position starting, following;
};

typedef struct Fragment YYLTYPE;

void print_frag(struct Fragment *f) {
    print_pos(&(f->starting));
    printf("-");
    print_pos(&(f->following));
}

union Token {
    char *ident; /* Бандитизм: должно быть int code ; */
    long num;
    char ch;
};

typedef union Token YYSTYPE;

int continued;
struct Position cur;

#define YY_USER_ACTION {                \
    int i;                              \
    if (!continued) {                   \
        yylloc->starting = cur;         \
    }                                   \
    continued = 0;                      \
                                        \
    for (i = 0; i < yyleng ; i++) {     \
        if (yytext[i] == '\n') {        \
            cur.line++;                 \
            cur.pos = 1;                \
        } else {                        \
            cur.pos++;                  \
        }                               \
        cur.index++;                    \
    }                                   \
                                        \
    yylloc->following = cur;            \
}


void init_scanner(char *program) {
    continued = 0;
    cur.line = 1;
    cur.pos = 1;
    cur.index = 0;
    yy_scan_string(program);
}

void err (char *msg) {
    /* Бандитизм : ошибки нужно класть в список ошибок . */
    printf("Error ");
    print_pos(&cur);
    printf(": %s\n", msg);
}
%}

LETTER      [a-zA-Z]
DIGIT       [0-9]
IDENT       {LETTER}({LETTER}|{DIGIT})*
NUMBER      {DIGIT}+

%x          COMMENTS CHAR_1 CHAR_2

%%

[\n\t ]+

\/\*                                BEGIN(COMMENTS); continued = 1;
<COMMENTS>[^*]*                     continued = 1;
<COMMENTS>\*\/                      {
                                        /* Бандитизм: координаты комментариев
                                            нужно класть в список комментариев. */
                                        print_frag(yylloc);
                                        printf(" comment\n");
                                        BEGIN(0);
                                    }
<COMMENTS>\*                        continued = 1;
<COMMENTS><<EOF>>                   {
                                        err("end of program found, '*/' expected");
                                        return 0;
                                    }

\(                                  return TAG_LPAREN;
\)                                  return TAG_RPAREN;
\+                                  return TAG_PLUS;
-                                   return TAG_MINUS;
\*                                  return TAG_MULTIPLY;
\/                                  return TAG_DIVIDE;

{IDENT}                             {
                                        /* Бандитизм: здесь нужно поместить
                                            идентификатор в словарь имён. */
                                        yylval->ident = yytext;
                                        return TAG_IDENT;
                                    }
{NUMBER}                            {
                                        long long num = atoi(yytext);
                                        if (strlen(yytext) > 10 || num >= LONG_MAX || num < 0) {
                                            err("number length overflow");
                                            BEGIN(0);
                                        } else {
                                            yylval->num = num;
                                            return TAG_NUMBER;
                                        }
                                    }
\’                                  BEGIN(CHAR_1); continued = 1;

.                                   err("unexpected character");

<CHAR_1,CHAR_2>\n                  {
                                        err("newline in constant");
                                        BEGIN(0);
                                        yylval->ch = 0;
                                        return TAG_CHAR;
                                    }
<CHAR_1>\\n                         yylval->ch = '\n'; BEGIN(CHAR_2); continued = 1;
<CHAR_1>\\\’                        yylval->ch = '\''; BEGIN(CHAR_2); continued = 1;
<CHAR_1>\\\\                        yylval->ch = '\\'; BEGIN(CHAR_2); continued = 1;
<CHAR_1>\\.                         {
                                        err("unrecognized Escape sequence");
                                        yylval->ch = 0;
                                        BEGIN(CHAR_2);
                                        continued = 1;
                                    }
<CHAR_1>\’                          {
                                        err("empty character literal");
                                        BEGIN(0);
                                        yylval->ch = 0;
                                        return TAG_CHAR;
                                    }
<CHAR_1>.                           yylval->ch = yytext[0]; BEGIN(CHAR_2); continued = 1;
<CHAR_2>\’                          BEGIN(0); return TAG_CHAR;
<CHAR_2>[^\n\’]*                    err("too many characters in literal"); continued = 1;

%%

#define PROGRAM "\
/* Expression */ (alpha + ’beta’ - ’\\n’)\n\
* 1234567890 /* blah-blah-blah\
"

int main () {
    int tag;
    YYSTYPE value;
    YYLTYPE coords;

    init_scanner(PROGRAM);

    do {
        tag = yylex(&value, &coords);
        switch (tag) {

            case TAG_IDENT:
                print_frag(&coords);
                printf(" IDENT %s\n", value.ident);
                break;

            case TAG_NUMBER:
                print_frag(&coords);
                printf(" NUMBER %li\n", value.num);
                break;

            case TAG_CHAR:
                print_frag(&coords);
                printf(" CHAR %i\n", value.ch);
                break;

            case TAG_LPAREN:
                print_frag(&coords);
                printf(" LPAREN\n");
                break;
            
            case TAG_RPAREN:
                print_frag(&coords);
                printf(" RPAREN\n");
                break;
            
            case TAG_PLUS:
                print_frag(&coords);
                printf(" PLUS\n");
                break;
            
            case TAG_MINUS:
                print_frag(&coords);
                printf(" MINUS\n");
                break;
            
            case TAG_MULTIPLY:
                print_frag(&coords);
                printf(" MULTIPLY\n");
                break;
            
            case TAG_DIVIDE:
                print_frag(&coords);
                printf(" DIVIDE\n");
                break;
        }
    } while (tag != 0);

    return 0;
}