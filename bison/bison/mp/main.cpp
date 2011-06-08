#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "mp-y.hpp"

varbl *varis;

int yyerror(char* s)
{
  printf("%s\n", s);
  return 0;
}

void clean()
{
  varbl *v;

  while (varis!=NULL)
  {
    if (!varis->type && varis->content.scnt)
        free(varis->content.scnt);
    v=varis;
    varis=varis->next;
    free(v);
    v=NULL;
  }

  return;
}

varbl* findvar(char *name)
{
  varbl *v=varis;

  while (v != NULL)
  {
    if (!strcmp(v->name, name)) break;
    v=v->next;
  }

  return v;
}

void readvar(char* name)
{
  varbl *v=findvar(name);

  switch (v->type)
  {
    case 0:
      v->content.scnt=(char*)calloc(128, sizeof(char));
      scanf("%s", v->content.scnt);
      break;
    case 1:
      scanf("%d", &(v->content.icnt));
      break;
  }
  return;
}

void newvar(char type, char* name)
{
  varbl *v=findvar(name), *v1;

  if (v)
  {
    fprintf(stderr, "The variable '%s' exists!\n", name);
    fflush(stderr);
    return;
  }

  v1=(varbl*)malloc(sizeof(varbl));
  v1->next=NULL;
  v1->type=type;
  v1->name=name;
  v1->content.scnt=NULL;

  v=varis;
  if (v == NULL) varis=v1; else
  {
    while (v->next != NULL) v=v->next;
    v->next=v1;
  }

  return;
}

void setivar(char* name, int what)
{
  varbl *v=findvar(name);

  errif(!v,, "Variable '%s' doesn't exist!\n", name);
  errif(v->type != 1,, "Incompatible types, %s isn't integral!\n", name);

  v->content.icnt=what;

  return;
}

void setsvar(char* name, char* what)
{
  varbl *v=findvar(name);

  errif(!v,, "Variable '%s' doesn't exist!\n", name);
  errif(v->type !=0,, "Incompatible types, '%s' isn't string!\n", name);

  v->content.scnt=what;

  return;
}

char* getvar(char* name)
{
  varbl *v=findvar(name);
  char *text=(char*)calloc(128, sizeof(char));

  text[0]=0;

  errif(!v, text, "Variable '%s' doesn't exist!\n", name);

  switch (v->type)
  {
    case 0:
      strcpy(text, v->content.scnt);
      break;
    case 1:
      sprintf(text, "%d", v->content.icnt);
      break;
  }

  return text;
}

void printhelp()
{
  puts("HELP\n");
  puts("\tThere needn't be brackets, there must be semicolon. String is in\
 quotes\n\tor apostrophes.\n");
  puts("Variables:");
  puts("\tstr $name-of-var;\t declaration of string variable");
  puts("\ti $name-of-var;\t\t declaration of integral variable\n");
  puts("Expressions:");
  puts("\tp(exp);\t\tprints exp, where exp is number, string or variable");
  puts("\tg($var);\tsets variable from standard input");
  puts("\t$var=exp;\tsets variable var to exp, where exp is number or string");
  puts("\tr(string);\truns the 'string'");
  puts("\th();\t\tthis help");
  puts("\tq();\t\tquit");

  return;
}
