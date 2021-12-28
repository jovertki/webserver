%{
#include <stdio.h>
#include "y.tab.h"
#include "lex_defines.h"
/*
GET						yylval=0; return METHOD;
HEAD					yylval=1; return METHOD;
POST					yylval=2; return METHOD;
PUT						yylval=3; return METHOD;
DELETE					yylval=4; return METHOD;
CONNECT					yylval=5; return METHOD;
OPTIONS					yylval=6; return METHOD;
TRACE					yylval=7; return METHOD;
PATCH					yylval=8; return METHOD;
*/
%}
%%
"GET"					return GET;
"HEAD"					return HEAD;
"POST"					return POST;
[ ][/][a-zA-Z0-9\/.-]?+        printf("URL\n");
HTTP[/][0-9][.][0-9]		printf("HTTPVER\n");

[Hh]"ost: "             printf("Host tkn = ");



\n                      /* ignore end of line */;
[ \t]+                  /* ignore whitespace */;
%%
