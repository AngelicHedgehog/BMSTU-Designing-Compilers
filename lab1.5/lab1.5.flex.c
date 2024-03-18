%option noyywrap bison-bridge bison-locations

%{

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

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
    size_t code;
    long num;
    char ch;
};

typedef union Token YYSTYPE;

int continued;
struct Position cur;
struct ErrorList errorList;
struct CommentList commentList;
struct NameDict nameDict;

#define YY_USER_ACTION {                \
    size_t i;                           \
    if (!continued) {                   \
        yylloc->starting = cur;         \
    }                                   \
    continued = 0;                      \
                                        \
    for (i = 0; i < yyleng ; ++i) {     \
        if (yytext[i] == '\n') {        \
            ++cur.line;                 \
            cur.pos = 1;                \
        } else {                        \
            ++cur.pos;                  \
        }                               \
        ++cur.index;                    \
    }                                   \
                                        \
    yylloc->following = cur;            \
}

struct Error {
    struct Position pos;
    char *msg;
};

struct ErrorList {
    struct Error *array;
    size_t length;
};

struct Comment {
    struct Fragment frag;
};

struct CommentList {
    struct Comment *array;
    size_t length;
};

struct Name {
    char *str;
};

struct NameDict {
    struct Name *array;
    size_t length;
};

void init_scanner(char *program) {
    continued = 0;
    
    cur.line = 1;
    cur.pos = 1;
    cur.index = 0;

    errorList.array = NULL;
    errorList.length = 0;
    
    commentList.array = NULL;
    commentList.length = 0;
    
    nameDict.array = NULL;
    nameDict.length = 0;

    yy_scan_string(program);
}

void err(char *msg) {
    ++errorList.length;
    errorList.array = (struct Error*)realloc(
        errorList.array, errorList.length * sizeof(struct Error));
    errorList.array[errorList.length - 1].pos = cur;
    errorList.array[errorList.length - 1].msg = msg;
}

void comm(struct Fragment *frag) {
    ++commentList.length;
    commentList.array = (struct Comment*)realloc(
        commentList.array, commentList.length * sizeof(struct Comment));
    commentList.array[commentList.length - 1].frag = *frag;
}

int containsName(char *name) {
    size_t i;
    for (i = 0; i != nameDict.length; ++i) {
        if (strcmp(nameDict.array[i].str, name) == 0) {
            return 1;
        }
    }
    return 0;
}

size_t addName(char *name) {
    ++nameDict.length;
    nameDict.array = (struct Name*)realloc(
        nameDict.array, nameDict.length * sizeof(struct Name));

    char **str = &nameDict.array[nameDict.length - 1].str;
    *str = malloc(strlen(name) + 1);
    strcpy(*str, name);

    return nameDict.length - 1;
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
                                        comm(yylloc);
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
                                        if (containsName(yytext) == 1) {
                                            err("this name alredy exists");
                                            BEGIN(0);
                                        } else {
                                            yylval->code = addName(yytext);
                                            return TAG_IDENT;
                                        }
                                    }
{NUMBER}                            {
                                        if ( strlen(yytext) < 10 ||
                                            (strlen(yytext) == 10 && strcmp(yytext, "2147483648") < 0))
                                        {
                                            yylval->num = atoi(yytext);
                                            return TAG_NUMBER;
                                        } else {
                                            err("number length overflow");
                                            BEGIN(0);
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
/* Expression */ (alpha * beta + alpha + ’beta’ - ’\\n’)\n\
* 1234567890 2147483648 /* blah-blah-blah\
"

int main () {
    size_t i;
    int tag;
    YYSTYPE value;
    YYLTYPE coords;

    init_scanner(PROGRAM);

    printf("IDENTS:\n");
    do {
        tag = yylex(&value, &coords);
        printf("\t");
        print_frag(&coords);
        printf(" %s", tag_names[tag]);
        switch (tag) {
            case TAG_IDENT:
                printf(" %s", nameDict.array[value.code].str);
                break;
            case TAG_NUMBER:
                printf(" %li", value.num);
                break;
            case TAG_CHAR:
                printf(" %i", value.ch);
                break;
        }
        printf("\n");
    } while (tag != 0);

    printf("ERRORS:\n");
    for (i = 0; i != errorList.length; ++i) {
        printf("\tError ");
        print_pos(&errorList.array[i].pos);
        printf(": %s\n", errorList.array[i].msg);
    }

    printf("COMMENTS:\n");
    for (i = 0; i != commentList.length; ++i) {
        printf("\t");
        print_frag(&commentList.array[i].frag);
        printf(" comment\n");
    }

    return 0;
}