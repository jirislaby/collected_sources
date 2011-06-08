#ifndef __MAIN_H__
#define __MAIN_H__

#define errif(podm, ret, pr, pr1) {\
  if (podm)\
  {\
    fprintf(stderr, pr, pr1);\
    fflush(stderr);\
    return ret;\
  }\
} 
 
int yylex();
int yyerror(char*);

void clean();
void printhelp();

void newvar(char, char*);
void setivar(char*, int);
void setsvar(char*, char*);
char* getvar(char*);
void readvar(char*);

typedef struct _varbls
{
  char *name;
  char type;
  union
  {
    int icnt;
    char *scnt;
  } content;
  struct _varbls *next;
} varbl;

varbl* findvar(char*);

extern varbl *varis;

#endif
