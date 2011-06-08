#include <err.h>
#include <stdio.h>

#include "lex.h"

int main(int argc, char **argv)
{
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (yyin == NULL) {
			err(1, "fopen");
		}
	}
	return yylex();
}
