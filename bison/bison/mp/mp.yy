%{
#include <string.h>
#include <stdlib.h>
#include "mp-y.hpp"
#include "main.h"

#define YY_NO_UNPUT
%}

CISLO [0-9]
ZNAK [^"']
ZNAKWOSP [^"' ;=\n)(]

%%

"str" { return STR; }

{CISLO}+ {
	yylval.val=atoi(yytext);
	return cislo;
}
"\""{ZNAK}*"\""|"'"{ZNAK}*"'" {
	yylval.str=(char*)calloc(strlen(yytext),sizeof(char));
	strncpy(yylval.str, yytext+1, strlen(yytext)-2);
	return retez;
}
"$"{ZNAKWOSP}+ {
	yylval.str=(char*)calloc(strlen(yytext), sizeof(char));
	strncpy(yylval.str, yytext+1, strlen(yytext)-1);
	return namevar;
}

\n|\t|" " {}
. {
/*	putchar(yytext[0]); */
	return yytext[0];
}

%%
