Program ::= (VarDecl | FuncDecl)* EOF

VarDecl ::= 'var' ID Type ('=' Expr)? ';'

FuncDecl ::= 'func' ID '(' ParamsList? ')' Type? Block

ParamsList ::= Param (',' Param)*

Param ::= 'ref'? ID Type

Type ::= 'int' | 'bool'

Block ::= '{' Stmt* '}'

Stmt ::= VarDecl
       | ID (ShortDecl | AssignStmt | CallStmt)
       | IfStmt
       | ForStmt
       | ReturnStmt
       | PrintStmt

ShortDecl ::= ':=' Expr ';'

AssignStmt ::= '=' Expr ';'

CallStmt ::= '(' ArgList? ')' ';'

IfStmt ::= 'if' Expr Block ('else' (IfStmt | Block))?

ForStmt ::= 'for' Expr Block

ReturnStmt ::= 'return' Expr? ';'

PrintStmt ::= ('print' | 'println') '(' PrintArg (',' PrintArg)* ')' ';'

PrintArg ::= Expr | STRING

ArgList ::= Arg (',' Arg)*

Arg ::= '&'? Expr

Expr ::= OrExpr

OrExpr ::= AndExpr ('||' AndExpr)*

AndExpr ::= NotExpr ('&&' NotExpr)*

NotExpr ::= '!' NotExpr | RelExpr

RelExpr ::= AddExpr (RelOp AddExpr)*

RelOp ::= RELATIONAL_OPERATOR

AddExpr ::= MulExpr (('+' | '-') MulExpr)*

MulExpr ::= UnaryExpr (('*' | '/' | '%') UnaryExpr)*

UnaryExpr ::= '-' UnaryExpr | Primary

Primary ::= INT
          | 'true'
          | 'false'
          | ID CallExpr?
          | '(' Expr ')'

CallExpr ::= '(' ArgList? ')'

---

* `ID` → identificador
* `INT` → literal entero
* `STRING` → literal string
* `RELATIONAL_OPERATOR` → `<`, `>`, `<=`, `>=`, `==`, `!=`
* `'&'` representa paso por referencia
* `':='` representa declaración corta


