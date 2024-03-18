%option noyywrap bison-bridge bison-locations

%{

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

enum TAGS {
    TAG_IDENT = 1,
    TAG_HEXNUMBER,
    TAG_QEQ,
    TAG_XX,
    TAG_XXX,
};

char *tag_names[] = {
    "END_OF_PROGRAM", "IDENT", "HEXNUMBER",
    "QEQ", "XX", "XXX",
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
    unsigned long long hexnum;
};

typedef union Token YYSTYPE;

int continued;
struct Position cur;
struct ErrorList errorList;
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
    size_t capacity;
};

void err(char *msg) {
    if (errorList.length == errorList.capacity) {
        errorList.capacity *= 2;
        errorList.array = (struct Error*)realloc(
            errorList.array, errorList.capacity * sizeof(struct Error));
    }

    ++errorList.length;
    errorList.array[errorList.length - 1].pos = cur;
    errorList.array[errorList.length - 1].msg = msg;
}

struct Name {
    char *str;
};

struct NameDict {
    struct Name *array;
    size_t length;
    size_t capacity;
};

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
    if (nameDict.length == nameDict.capacity) {
        nameDict.capacity *= 2;
        nameDict.array = (struct Name*)realloc(
            nameDict.array, nameDict.capacity * sizeof(struct Name));
    }

    ++nameDict.length;
    char **str = &nameDict.array[nameDict.length - 1].str;
    *str = malloc(strlen(name) + 1);
    strcpy(*str, name);

    return nameDict.length - 1;
}

void init_scanner(char *program) {
    continued = 0;
    
    cur.line = 1;
    cur.pos = 1;
    cur.index = 0;

    errorList.array = NULL;
    errorList.length = 0;
    errorList.capacity = 0;
    
    nameDict.array = NULL;
    nameDict.length = 0;
    nameDict.capacity = 0;

    yy_scan_string(program);
}

%}

LETTER      [a-zA-Z]
DIGIT       [0-9]
HEXDIGIT    [a-hA-F0-9]
IDENT       {LETTER}({DIGIT}*{LETTER})*
HEXNUMBER   0{HEXDIGIT}*

%%

[\n\t ]+
qeq                                 return TAG_QEQ;
xx                                  return TAG_XX;
xxx                                 return TAG_XXX;

{IDENT}                             {
                                        if (containsName(yytext) == 1) {
                                            err("this name alredy exists");
                                            BEGIN(0);
                                        } else {
                                            yylval->code = addName(yytext);
                                            return TAG_IDENT;
                                        }
                                    }
{HEXNUMBER}                         {
                                        if ( strlen(yytext) < 17 )
                                        {
                                            yylval->hexnum = strtoull(yytext, NULL, 16);
                                            return TAG_HEXNUMBER;
                                        } else {
                                            err("number length overflow");
                                            BEGIN(0);
                                        }
                                    }

.                                   err("unexpected character"); BEGIN(0);

%%

#define PROGRAM "0fabc13 qeq  qe0e x xxx xx "

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
            case TAG_HEXNUMBER:
                printf(" 0x%X", value.hexnum);
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

    return 0;
}
