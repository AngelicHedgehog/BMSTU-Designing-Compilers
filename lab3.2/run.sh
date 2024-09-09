#!/bin/bash

lex -o lexer.c lexer.l.c
yacc -d -o parser.c parser.y.c
gcc -o calc lexer.c parser.c

./calc $1

rm -f parser.c parser.h
rm -f lexer.c lexer.h
rm -f calc
