#include <string>
#include"parser.hpp"

static int indentLevel = 0;
static inline std::string indentSpaces() {
    return std::string(indentLevel * 2, ' ');
}

struct IndentGuard {
    bool active;
    IndentGuard(bool active_) : active(active_) {
        if(active) ++indentLevel;
    }
    ~IndentGuard() {
        if(active) --indentLevel;
    }
};

//Init -> Program
void Parser::parse(const bool text){
    parseProgram(text);
}

void Parser::debugEnter(std::string_view nonterminal, const bool text) const {
    if(text) std::cout << indentSpaces() << "Entering " << nonterminal << std::endl;
}

void Parser::printCurrent(const bool text) const {
    if(text) std::cout << indentSpaces() << "Current: " << current.txt << std::endl;
}

// Program -> (VarDecl | FuncDecl)* EOF
void Parser::parseProgram(const bool text){
    debugEnter("parseProgram", text);
    IndentGuard guard(text);
    while(equal(Token::KEYWORD, "var") || equal(Token::KEYWORD, "func")){
        if(current.txt == "var"){
            parseVarDecl(text);
        } else {
            parseFuncDecl(text);
        }
    }
    match(Token::END_OF_FILE, text, "parseProgram");
}

//VarDecl -> KEYWORD('var') ID KEYWORD('int' | 'bool') (ASSIGN Expr)? PUNCTUATION(';')
void Parser::parseVarDecl(const bool text){
    debugEnter("parseVarDecl", text);
    IndentGuard guard(text);
    // KEYWORD('var')
    if(equal(Token::KEYWORD, "var")){
        consume(text);
        // ID
        match(Token::ID, text, "parseVarDecl");
        
        // KEYWORD('int' | 'bool')
        if(equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")){
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseVarDecl: expected type 'int' or 'bool', but got: {}", current.txt));
        }
        // (ASSIGN Expr)?
        if(equal(Token::ASSIGN)){
            consume(text);
            parseExpr(text);
        }
        // PUNCTUATION(';')
        match(Token::PUNCTUATIONAL, ";", text, "parseVarDecl");
    }else {
        throw std::runtime_error(std::format("Error in parseVarDecl: expected 'var' keyword, but got: {}", current.txt));
    }
}

//FuncDecl -> KEYWORD('func') ID PUNCTUATION('(') (Paramslist)? PUNCTUATION(')') (KEYWORD('int' | 'bool'))? Block 
void Parser::parseFuncDecl(const bool text){
    debugEnter("parseFuncDecl", text);
    IndentGuard guard(text);
    if(equal(Token::KEYWORD, "func")){
        consume(text);
        // ID
        match(Token::ID, text, "parseFuncDecl");
        // PUNCTUATION('(')
        if(equal(Token::PUNCTUATIONAL, "(")){
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseFuncDecl: expected '(', but got: {}", current.txt));
        }
        // (Paramslist)?
        if(equal(Token::ID) || equal(Token::KEYWORD, "ref")){
            parseParamsList(text);
        }
        // PUNCTUATION(')')
        if(equal(Token::PUNCTUATIONAL, ")")){
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseFuncDecl: expected ')', but got: {}", current.txt));
        }

        // KEYWORD('int' | 'bool')?
        if(equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")){
            consume(text);
        }
        
        //Block
        parseBlock(text);
    }else {
        throw std::runtime_error(std::format("Error in parseFuncDecl: expected 'func' keyword, but got: {}", current.txt));
    }
}

// ParamsList -> Param (PUNCTUATION(',') Param)*
void Parser::parseParamsList(const bool text){
    debugEnter("parseParamsList", text);
    IndentGuard guard(text);
    //Param
    parseParam(text);
    // (PUNCTUATION(',') Param)*
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        parseParam(text);
    }
}

// Param -> (KEYWORD('ref'))? ID KEYWORD('int' | 'bool')
void Parser::parseParam(const bool text){
    debugEnter("parseParam", text);
    IndentGuard guard(text);
    // (KEYWORD('ref'))?
    if(equal(Token::KEYWORD,"ref")){
        consume(text);
    }

    // ID
    match(Token::ID, text, "parseParam");
    // KEYWORD('int' | 'bool')
    if(equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")){
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseParam: expected type 'int' or 'bool', but got: {}", current.txt));
    }
}

// Block -> PUNCTUATION('{') (Stmt)* PUNCTUATION('}')
void Parser::parseBlock(const bool text){
    debugEnter("parseBlock", text);
    IndentGuard guard(text);
    // PUNCTUATION('{')
    match(Token::PUNCTUATIONAL, "{", text, "parseBlock");
    // (Stmt)*
    while(equal(Token::KEYWORD, "var") || equal(Token::ID) || 
            equal(Token::KEYWORD, "if") || equal(Token::KEYWORD, "for") ||
            equal(Token::KEYWORD, "return") || equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")
        ){
        parseStmt(text);
    }     
    // PUNCTUATION('}')
    if(equal(Token::PUNCTUATIONAL, "}")){
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseBlock: expected '}}', but got: {}", current.txt));
    }
}

// Stmt -> VarDecl|ID (ShortDecl|AssignStmt|CallStmt) | ifStmt | forStmt | returnStmt | printStmt
void Parser::parseStmt(const bool text){
    debugEnter("parseStmt", text);
    IndentGuard guard(text);
    if(equal(Token::KEYWORD, "var")){
        parseVarDecl(text);
    } else if(equal(Token::ID)){
        consume(text);
        if(equal(Token::SHORT_ASSIGN)){
            parseShortDecl(text);
        }else if(equal(Token::ASSIGN)){
            parseAssignStmt(text);
        } else if(equal(Token::PUNCTUATIONAL, "(")){
            parseCallStmt(text);
        } else {
            throw std::runtime_error(std::format("Error in parseStmt: expected ':=', '=', or '(', but got: {}", current.txt));
        }
    } else if(equal(Token::KEYWORD, "if")){
        parseIfStmt(text);
    } else if(equal(Token::KEYWORD, "for")){
        parseForStmt(text);
    } else if (equal(Token::KEYWORD, "return")){
        parseReturnStmt(text);
    } else if (equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")){
        parsePrintStmt(text);
    } else {
        throw std::runtime_error(std::format("Error in parseStmt: unexpected token: {}", current.txt));
    }
}

// ShortDecl -> SHORT_ASSIGN Expr PUNCTUATION(';')
void Parser::parseShortDecl(const bool text){
    debugEnter("parseShortDecl", text);
    IndentGuard guard(text);
    // SHORT_ASSIGN
    match(Token::SHORT_ASSIGN, text);
    parseExpr(text);
    // PUNCTUATION(';')
    match(Token::PUNCTUATIONAL, ";", text);
}

// AssignStmt -> ASSIGN Expr SEMICOLON
void Parser::parseAssignStmt(const bool text){
    debugEnter("parseAssignStmt", text);
    IndentGuard guard(text);
    // ASSIGN
    match(Token::ASSIGN, text);
    // Expr
    parseExpr(text);
    // SEMICOLON
    match(Token::PUNCTUATIONAL, ";", text);
}

// CallStmt -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')') SEMICOLON
void Parser::parseCallStmt(const bool text){
    debugEnter("parseCallStmt", text);
    IndentGuard guard(text);
    // PUNCTUATION('(')
    match(Token::PUNCTUATIONAL, "(", text, "parseCallStmt");
    // (ArgList)?
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        parseArgList(text);
    }
    // PUNCTUATION(')')
    match(Token::PUNCTUATIONAL, ")", text, "parseCallStmt");
    // SEMICOLON
    match(Token::PUNCTUATIONAL,";", text, "parseCallStmt");
}

// ifStmt -> KEYWORD('if') Expr Block (KEYWORD('else')(ifStmt | Block))? 
void Parser::parseIfStmt(const bool text){
    debugEnter("parseIfStmt", text);
    IndentGuard guard(text);
    match(Token::KEYWORD, "if", text, "parseIfStmt");
    parseExpr(text);
    parseBlock(text);
    if(equal(Token::KEYWORD, "else")){
        consume(text);
        if(equal(Token::KEYWORD, "if")){
            parseIfStmt(text);
        } else {
            parseBlock(text);
        }
    }
};
// forStmt -> KEYWORD('for') Expr Block
void Parser::parseForStmt(const bool text){
    debugEnter("parseForStmt", text);
    IndentGuard guard(text);
    match(Token::KEYWORD, "for", text, "parseForStmt");
    parseExpr(text);
    parseBlock(text);
};
// returnStmt -> KEYWORD('return') (Expr)? SEMICOLON
void Parser::parseReturnStmt(const bool text){
    debugEnter("parseReturnStmt", text);
    IndentGuard guard(text);
    match(Token::KEYWORD, "return", text, "parseReturnStmt");
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        parseExpr(text);
    }
    match(Token::PUNCTUATIONAL,";", text, "parseReturnStmt");
};
// printStmt -> (KEYWORD('print') | KEYWORD('println')) PUNCTUATION('(') PrintArg (PUNCTUATION(',') PrintArg)* PUNCTUATION(')') SEMICOLON
void Parser::parsePrintStmt(const bool text){
    debugEnter("parsePrintStmt", text);
    IndentGuard guard(text);
    // (KEYWORD('print') | KEYWORD('println'))
    if(equal(Token::KEYWORD,"print") || equal(Token::KEYWORD,"println")){
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parsePrintStmt: expected 'print' or 'println', but got: {}", current.txt));
    }

    // PUNCTUATION('(')
    match(Token::PUNCTUATIONAL, "(", text, "parsePrintStmt");
    // PrintArg
    parsePrintArg(text);
    // (PUNCTUATION(',') PrintArg)*
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        parsePrintArg(text);
    }
    // PUNCTUATION(')')
    match(Token::PUNCTUATIONAL, ")", text, "parsePrintStmt");
    // SEMICOLON
    match(Token::PUNCTUATIONAL,";", text, "parsePrintStmt");

};
// PrintArg -> Expr | STRING
void Parser::parsePrintArg(const bool text){
    debugEnter("parsePrintArg", text);
    IndentGuard guard(text);
    if(equal(Token::STRING)){
        consume(text);
    } else {
        parseExpr(text);
    }
};
// ArgList -> Arg (PUNCTUATION(',') Arg)*
void Parser::parseArgList(const bool text){
    debugEnter("parseArgList", text);
    IndentGuard guard(text);
    parseArg(text);
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        parseArg(text);
    }
};

// Arg -> (ADDRESS)? Expr
void Parser::parseArg(const bool text){
    debugEnter("parseArg", text);
    IndentGuard guard(text);
    // (ADDRESS)?
    if(equal(Token::ADDRESS)){
        consume(text);
    }
    // Expr
    parseExpr(text);
}

// Expr -> OrExpr
void Parser::parseExpr(const bool text){
    debugEnter("parseExpr", text);
    IndentGuard guard(text);
    parseOrExpr(text);
}

// OrExpr -> AndExpr (LOGICAL('||') andExpr)*
void Parser::parseOrExpr(const bool text){
    debugEnter("parseOrExpr", text);
    IndentGuard guard(text);
    // AndExpr
    parseAndExpr(text);
    // (LOGICAL('||') andExpr)*
    while(equal(Token::LOGICAL, "||")){
        consume(text);
        parseAndExpr(text);
    }
}

// AndExpr -> NotExpr (LOGICAL('&&') NotExpr)*
void Parser::parseAndExpr(const bool text){
    debugEnter("parseAndExpr", text);
    IndentGuard guard(text);
    // NotExpr
    parseNotExpr(text);
    // (LOGICAL('&&') NotExpr)*
    while(equal(Token::LOGICAL, "&&")){
        consume(text);
        parseNotExpr(text);
    }
};

// NotExpr -> LOGICAL('!') NotExpr | RelExpr
void Parser::parseNotExpr(const bool text){
    debugEnter("parseNotExpr", text);
    IndentGuard guard(text);
    if(equal(Token::LOGICAL, "!")){
        consume(text);
        parseNotExpr(text);
    } else {
        parseRelExpr(text);
    }
};

// RelExpr -> AddExpr ((RelOp) AddExpr)*
void Parser::parseRelExpr(const bool text){
    debugEnter("parseRelExpr", text);
    IndentGuard guard(text);
    // AddExpr
    parseAddExpr(text);
    // ((RelOp) AddExpr)*
    while(equal(Token::RELATIONAL)){
        parseRelOp(text);
        parseAddExpr(text);
    }
};

// RelOp -> RELATIONAL(any)
void Parser::parseRelOp(const bool text){
    debugEnter("parseRelOp", text);
    IndentGuard guard(text);
    match(Token::RELATIONAL, text);
};

// AddExpr -> MulExpr ((ARITH('+')| ARITH('-')) MulExpr)*
void Parser::parseAddExpr(const bool text){
    debugEnter("parseAddExpr", text);
    IndentGuard guard(text);
    // MulExpr
    parseMulExpr(text);
    // ((ARITH('+')| ARITH('-')) MulExpr)*
    while(equal(Token::ARITH, "+") || equal(Token::ARITH, "-")){
        consume(text);
        parseMulExpr(text);
    }
};

// MulExpr -> UnaryExpr ((ARITH('*')| ARITH('/')| ARITH('%')) UnaryExpr)*
void Parser::parseMulExpr(const bool text){
    debugEnter("parseMulExpr", text);
    IndentGuard guard(text);
    // UnaryExpr
    parseUnaryExpr(text);
    // ((ARITH('*')| ARITH('/')| ARITH('%')) UnaryExpr)*
    while(equal(Token::ARITH,"*") || equal(Token::ARITH,"/") || equal(Token::ARITH,"%")){
        consume(text);
        parseUnaryExpr(text);
    }
};

// UnaryExpr -> ARITH('-') UnaryExpr | Primary
void Parser::parseUnaryExpr(const bool text){
    debugEnter("parseUnaryExpr", text);
    IndentGuard guard(text);
    if(equal(Token::ARITH, "-")){
        consume(text);
        parseUnaryExpr(text);
    } else {
        parsePrimary(text);
    }
};

// Primary -> INT | KEYWORD('true') | KEYWORD('false') | ID (CallExpr)? | PUNCTUATION('(') Expr PUNCTUATION(')')
void Parser::parsePrimary(const bool text){
    debugEnter("parsePrimary", text);
    IndentGuard guard(text);
    if(equal(Token::INT)){
        consume(text);
    } else if(equal(Token::KEYWORD, "true")){
        consume(text);
    } else if(equal(Token::KEYWORD, "false")){
        consume(text);
    } else if(equal(Token::ID)){
        consume(text);
        if(equal(Token::PUNCTUATIONAL,"(")){
            parseCallExpr(text);
        }
    } else if(equal(Token::PUNCTUATIONAL, "(")){
        consume(text);
        parseExpr(text);
        match(Token::PUNCTUATIONAL, ")", text);
    } else {
        throw std::runtime_error(std::format("Unexpected token in primary expression: {}", current.txt));
    }
};

// CallExpr -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')')
void Parser::parseCallExpr(const bool text){
    debugEnter("parseCallExpr", text);
    IndentGuard guard(text);
    match(Token::PUNCTUATIONAL,"(", text);
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        parseArgList(text);
    }
    match(Token::PUNCTUATIONAL, ")", text);
};  