grammar T;
options {
    output=AST;
    // ANTLR can handle literally any tree node type.
    // For convenience, specify the Java type
    ASTLabelType=CommonTree; // type of $stat.tree ref etc...
}
tokens {
	BLOCK;
	DECL;
	VARDEF;
}

root:	prog!? block
	;

prog:	'program' ID ';'
	;

block:	('var' decl*)? 'begin' stat* 'end'	-> ^(BLOCK decl* stat*)
	;

decl	: def ';'!
	;

stat	: line ';'!
	;

def	: ID ':' type				-> ^(VARDEF type ID)
	;

line	: ID '=' expr				-> ^('=' ID expr)
	| 'print' expr				-> ^('print' expr)
	;

expr	: exps ('+'^ exps)*
	;

exps	: expm ('-'^ expm)*
	;

expm	: expd ('*'^ expd)*
	;

expd	: exp ('/'^ exp)*
	;

exp	: ID
	| INT
	| REAL
	| '('! expr ')'!
	;

type	: 'int'
	| 'real'
	;

COMMENT	: '/*' (options {greedy=false;} : .)* '*/'	{ $channel=HIDDEN; }
	;

ID	: ('a'..'z'|'A'..'Z') ('_'|'0'..'9'|'a'..'z'|'A'..'Z')*
	;

INT	: ('0'..'9')+
	;

REAL	: INT? '.' INT
	;

STRING	: '"' ('.'|','|':'|';'|'_'|'0'..'9'|'a'..'z'|'A'..'Z')* '"'
	;

WS	: (' ' | '\t' | '\n' |'\r' )+			{ $channel=HIDDEN; }
	;
