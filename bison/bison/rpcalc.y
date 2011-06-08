%{
#define YYSTYPE int 
#define YYDEBUG 1
#include <ctype.h>
#include <stdio.h>

int yylex();
int yyerror(char*);

%}

%token NUM

%%

input:
	| '\n' | input xxx '\n'
;
xxx:	NUM  {printf ("\t%d\n",$1); } | '('xxx')'
;
%%

int yylex ()
{
  int c;

  while ((c = getchar ()) == ' ' || c == '\t');

  if (isdigit (c))                
    {
      ungetc (c, stdin);
      scanf ("%d", &yylval);
      return NUM;
    }

  if (c == EOF) return 0;
  
  return c;                                
}

int main ()
{
  yyparse ();
  return 0;
}


int yyerror (char* s)
{
  printf ("%s\n", s);
  return 0;
}
