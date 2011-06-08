%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

int yylex();
int yyerror(char*);
void printhelp();

%}

%union {
  char* str;
  int val;
}

%token <str> retez 
%token <val> cislo

%type <str> prexp
%type <str> prnpar
%%
input:
	| 'q' brs ';' { exit(0); }
	| print input
	| help input
;

help:	'h' brs ';' { printhelp();}
;

print:	'p' lbr prexp rbr ';' {printf("%s\n",$3);}
;
prexp:	prnpar retez { $$=$1; strcat($$,$2); }
	| prnpar cislo {
}
;
prnpar:
	| prexp ',' {$$=$1;}
;

brs:
	| '(' ')'
;
lbr:
	| '('
;
rbr:
	| ')'
;
%%

int yyerror(char *s)
{
	printf("%s\n",s);
	return 0;
}

int yylex()
{
	int c;
//	int moje;
	
	while ((c=getchar()) == ' ' || c == '\t' || c == '\n');
		
	if (c == EOF) return 0;

	if (isdigit(c))
	{
	  ungetc(c,stdin);
	  scanf("%d",&yylval.val);
	  return cislo;
	}
	if (c=='"')
	{
	  unget(c,stdin);
	  scanf("\"%s\"",yylval.str);
	  return retez;
	}

	return c;
}

int main()
{
  yyparse();
  return 0;
}

void printhelp()
{
 	puts("\tHELP\n");
	puts("\tThere couldn't be brackets, there must be semicolon");
	puts("\tp(exp);\tprints exp, where exp is number or string");
	puts("\th();\tthis help");
	puts("\tq();\tquit");
	return;
}
