%{
#define YYSTYPE char*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp-y.hpp"
#include "main.h"

extern int line;

int yylex();
void yyerror(const char*);
int main(int, char**);

%}

%start input

%token NL

%token STR
%token NUM

%%
input:
	| input line		{ printf("++%s\n", $2); }
	| input error NL	{ puts("Chyba na radku"); yyerrok; }
;
line:	STR ' ' NUM NL	{ $$=$1; }
	| NUM NL	{ $$="b"; }
;

%%

int main(int argc, char** argv)
{
  line = 1;

  yyparse();

  return 0;
}

void yyerror(const char* s)
{
  printf("%s on line %d\n", s, line);
}
