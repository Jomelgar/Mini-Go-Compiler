%%
[{]|[}]|"("|")"|,|; {return PUNC}
& {return ADDRESS}
&&|"||"|! {return LOGICAL}
-?[0-9]+ {return INT}
"=" {return ASSIGN};
":=" {return SHORT_ASSIGN}
\"([^"\\\n]|\\[\\"nt])*\" {return STRING}
"=="|"!="|[<>][=]? {return REL_OP}
[+\-\/*%] {return ARITH_OP}
[a-zA-Z_][a-zA-Z_0-9]* {return ID;}
[ ]|\n|\t|("//".*)|("/*"([^*]|\*[^\/])*"*/") {return IGNORE}
<<EOF>> {return EOF}
. {return UNK;}
%%