#!bin/bash
lex example5.l
yacc -d example5.y
cc lex.yy.c y.tab.c -o example5 
