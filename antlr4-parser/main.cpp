#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "antlr4-runtime.h"
#include "parLexer.h"
#include "parParser.h"

int main(int argc, char **argv)
{
	antlr4::ANTLRInputStream input(std::cin);
	parLexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	parParser parser(&tokens);
	antlr4::tree::ParseTree* tree = parser.tu();

	std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

	return 0;
}

