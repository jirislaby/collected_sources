%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mp-y.hpp"
#include "main.h"

%}

%union {
  char* str;
  int val;
}

%start input

%token <str> namevar 
%token <str> retez
%token <val> cislo

%type <str> prexp
%token STR

%%
input:
	| input 'q' brs ';' { exit(0); }
	| input 'h' brs ';' { printhelp(); }
	| input print
	| input vars
	| input run
;

run: 'r' lbr retez rbr ';' { printf("Result: %d\n", system($3)); }
;

print:	'p' lbr prexp rbr ';' { printf("%s\n", $3); free($3); }
;
prexp:	{ $$=""; }
	| cislo {
		$$=(char*)calloc((int)log10($1)+1,sizeof(char));
		sprintf($$, "%d", $1);
	}
	| retez { $$=$1; }
	| namevar { $$=getvar($1); }
;

vars:	'i' namevar ';' { newvar(1, $2); }
	| STR namevar ';' { newvar(0, $2); }
	| namevar '=' cislo ';' { setivar($1, $3); }
	| namevar '=' retez ';' { setsvar($1, $3); }
	| 'g' lbr namevar rbr ';' { readvar($3); }
;

lbr:
	| '('
;
rbr:
	| ')'
;
brs: lbr rbr
;
%%

int main(int argc, char** argv)
{
  int a=1;
  extern FILE *yyin;

  varis=NULL;

  if (argc == 1) 
  {
    yyparse();
    clean();
  }
  else
  for (; a<argc; a++)
  {
    yyin=fopen(argv[a], "r");
    yyparse();
    fclose(yyin);
    clean();
  }

  return 0;
}
