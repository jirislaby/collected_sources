%{
#include <stdio.h>
#include <string.h>

static unsigned int line;
static unsigned int paren;
static char *string;

static inline void print_string()
{
	size_t len = strlen(string);
	unsigned int a;

	putchar('"');
	for (a = 0; a < len; a++)
		if (string[a] == '%' && string[a + 1] == 'P' &&
				string[a + 2] >= '0' && string[a + 2] <= '9') {
			putchar('%');
			putchar('p');
			putchar(string[a + 2]);
			a += 2;
		} else
			putchar(string[a]);
	putchar('"');
	free(string);
}
%}

%option noinput
%option nounput

%x ASM
%x ASM_TEMPL
%x ASM_TEMPL_STR
%x ASM_REST

%%

<INITIAL>"asm"[ \t]*"volatile"?[ \t]*"("	{
		ECHO;
		BEGIN(ASM_TEMPL);
//		fprintf(stderr, "asm at %u\n", line);
		string = malloc(1000);
		if (!string)
			abort();
		string[0] = 0;
	}

<ASM_TEMPL>"\""		{ BEGIN(ASM_TEMPL_STR); }
<ASM_TEMPL>":"		{ print_string(); putchar(' '); ECHO; BEGIN(ASM_REST); }
<ASM_TEMPL>")"		{ print_string(); ECHO; BEGIN(INITIAL); }
<ASM_TEMPL>[ \t]

<ASM_TEMPL_STR>"\\\""	{ strcat(string, yytext); }
<ASM_TEMPL_STR>"\""	{ BEGIN(ASM_TEMPL); }
<ASM_TEMPL_STR>.	{ strcat(string, yytext); }

<ASM_REST>"("		{ ECHO; paren++; }
<ASM_REST>")"		{ ECHO; if (paren == 0) BEGIN(INITIAL); paren--; }

<*>"\n"			{ ECHO; line++; }

<*>.			{ ECHO; }

%%
