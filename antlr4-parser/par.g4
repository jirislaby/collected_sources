grammar par;

tu: (stmt ';' { std::cout << $stmt.ret << std::endl; } )*
	;

stmt returns [long ret]: e1=expr '+' e2=expr	{ $ret = $e1.ret + $e2.ret; }
	| e1=expr '-' e2=expr			{ $ret = $e1.ret - $e2.ret; }
	| expr					{ $ret = $expr.ret; }
	; 

expr returns [long ret=0]:
	'ahoj'
	| 'vogo'
	| '-' NUM		{ $ret = -std::stol($NUM.text); }
	| NUM			{ $ret = std::stol($NUM.text); }
	;

NUM: [0-9]+ ;

WS: [\r\n ]+ -> channel(HIDDEN) ;
