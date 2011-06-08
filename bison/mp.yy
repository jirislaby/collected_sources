%{
#define YYSTYPE char *
#include <string.h>
#include <stdlib.h>
#include "mp-y.hpp"
#include "main.h"

#define YY_NO_UNPUT

#define RET(what) do { yylval=yytext; return what; } while (0)

#ifdef debug
  #define prdeb(what) printf("--%s: '%s'\n",what,yytext)
#else
  #define prdeb(what) do {} while (0);
#endif

int main(int, char**);

int line;

%}

%option always-interactive

NUMa [0-9]+
STRa [^\n ]+

%%

{NUMa}		{ prdeb("num"); RET(NUM); }
{STRa}		{ prdeb("str") RET(STR); }

"\n"		{ return NL; }

.		{ prdeb("tecka") return yytext[0]; }

%%
