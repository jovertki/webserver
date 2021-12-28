#!/bin/bash
lex lex_test1.l
cc lex.yy.c -o a.out -ll
./a.out < test_request