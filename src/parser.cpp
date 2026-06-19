#include <string>
#include"../include/parser.hpp"

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
std::vector<ASTNode *> Parser::parse(const bool text){
    return parseProgram(text);
}

void Parser::debugEnter(std::string_view nonterminal, const bool text) const {
    if(text) std::cout << indentSpaces() << "Entering " << nonterminal << std::endl;
}

void Parser::printCurrent(const bool text) const {
    if(text) std::cout << indentSpaces() << "Current: " << current.txt << std::endl;
}

// Program -> (VarDecl | FuncDecl)* EOF
std::vector<ASTNode *> Parser::parseProgram(const bool text){
    std::vector<ASTNode *> program;
    while(equal(Token::KEYWORD, "var") || equal(Token::KEYWORD, "func")){
        if(current.txt == "var"){
            program.push_back(parseVarDecl(text));
        } else {
            program.push_back(parseFuncDecl(text));
        }
    }
    match(Token::END_OF_FILE, text, "parseProgram");
    return program;
}

//VarDecl -> KEYWORD('var') ID KEYWORD('int' | 'bool') (ASSIGN Expr)? PUNCTUATION(';')
vardecl * Parser::parseVarDecl(const bool text){
    debugEnter("parseVarDecl", text);
    IndentGuard guard(text);

    std::string id = current.txt;
    Type type = Type::VOID;
    Expr * expr = nullptr;
    // KEYWORD('var')
    if(equal(Token::KEYWORD, "var")){
        consume(text);
        // ID
        match(Token::ID, text, "parseVarDecl");
        // KEYWORD('int' | 'bool')
        if(equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")){
            type = equal(Token::KEYWORD, "int") ? Type::INT : Type::BOOL;
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseVarDecl: expected type 'int' or 'bool', but got: {}", current.txt));
        }
        // (ASSIGN Expr)?
        if(equal(Token::ASSIGN)){
            consume(text);
            expr = parseExpr(text);
        }
        // PUNCTUATION(';')
        match(Token::PUNCTUATIONAL, ";", text, "parseVarDecl");
    }else {
        throw std::runtime_error(std::format("Error in parseVarDecl: expected 'var' keyword, but got: {}", current.txt));
    }

    return new vardecl(type,id, expr);
}

//FuncDecl -> KEYWORD('func') ID PUNCTUATION('(') (Paramslist)? PUNCTUATION(')') (KEYWORD('int' | 'bool'))? Block 
funcdecl * Parser::parseFuncDecl(const bool text){
    debugEnter("parseFuncDecl", text);
    IndentGuard guard(text);

    std::vector<param *> params;
    Block * block;
    std::string id;
    if(equal(Token::KEYWORD, "func")){
        consume(text);
        // ID
        id = current.txt;
        match(Token::ID, text, "parseFuncDecl");
        // PUNCTUATION('(')
        if(equal(Token::PUNCTUATIONAL, "(")){
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseFuncDecl: expected '(', but got: {}", current.txt));
        }
        // (Paramslist)?
        if(equal(Token::ID) || equal(Token::KEYWORD, "ref")){
            params = parseParamsList(text);
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
        block = parseBlock(text);
    }else {
        throw std::runtime_error(std::format("Error in parseFuncDecl: expected 'func' keyword, but got: {}", current.txt));
    }

    return new funcdecl(block, params, id);
}

// ParamsList -> Param (PUNCTUATION(',') Param)*
std::vector<param*> Parser::parseParamsList(const bool text){
    debugEnter("parseParamsList", text);
    IndentGuard guard(text);
    std::vector<param *> params;
    //Param
    params.push_back(parseParam(text));
    // (PUNCTUATION(',') Param)*
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        params.push_back(parseParam(text));
    }
    return params;
}

// Param -> (KEYWORD('ref'))? ID KEYWORD('int' | 'bool')
param* Parser::parseParam(const bool text){
    debugEnter("parseParam", text);
    IndentGuard guard(text);
    Type type;
    std::string id;
    bool isRef = false;
    // (KEYWORD('ref'))?
    if(equal(Token::KEYWORD,"ref")){
        consume(text);
        isRef = true;
    }

    // ID
    id = current.txt;
    match(Token::ID, text, "parseParam");
    // KEYWORD('int' | 'bool')
    if(equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")){
        type = equal(Token::KEYWORD, "int") ? Type::INT : Type::BOOL;
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseParam: expected type 'int' or 'bool', but got: {}", current.txt));
    }

    return new param(type, id, isRef);
}

// Block -> PUNCTUATION('{') (Stmt)* PUNCTUATION('}')
Block * Parser::parseBlock(const bool text){
    std::vector<ASTNode*> stmts;
    // PUNCTUATION('{')
    match(Token::PUNCTUATIONAL, "{", text, "parseBlock");
    // (Stmt)*
    while(equal(Token::KEYWORD, "var") || equal(Token::ID) || 
            equal(Token::KEYWORD, "if") || equal(Token::KEYWORD, "for") ||
            equal(Token::KEYWORD, "return") || equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")
        ){
        stmts.push_back(parseStmt(text));
    }     
    // PUNCTUATION('}')
    if(equal(Token::PUNCTUATIONAL, "}")){
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseBlock: expected '}}', but got: {}", current.txt));
    }

    return new Block(stmts);
}

// Stmt -> VarDecl|ID (ShortDecl|AssignStmt|CallStmt) | ifStmt | forStmt | returnStmt | printStmt
stmt * Parser::parseStmt(const bool text){
    debugEnter("parseStmt", text);
    IndentGuard guard(text);
    if(equal(Token::KEYWORD, "var")){
        return parseVarDecl(text);
    } else if(equal(Token::ID)){
        std::string id = current.txt;
        consume(text);
        if(equal(Token::SHORT_ASSIGN)){
            parseShortDecl(id);
        }else if(equal(Token::ASSIGN)){
            parseAssignStmt(id);
        } else if(equal(Token::PUNCTUATIONAL, "(")){
            parseCallStmt(id);
        } else {
            throw std::runtime_error(std::format("Error in parseStmt: expected ':=', '=', or '(', but got: {}", current.txt));
        }
    } else if(equal(Token::KEYWORD, "if")){
        return parseIfStmt(text);
    } else if(equal(Token::KEYWORD, "for")){
        return parseForStmt(text);
    } else if (equal(Token::KEYWORD, "return")){
        return parseReturnStmt(text);
    } else if (equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")){
        return parsePrintStmt(text);
    }
    
    throw std::runtime_error(std::format("Error in parseStmt: unexpected token: {}", current.txt));
}

// ShortDecl -> SHORT_ASSIGN Expr PUNCTUATION(';')
shortdecl * Parser::parseShortDecl(const std::string id){
    Expr * expr = nullptr;
    // SHORT_ASSIGN 
    match(Token::SHORT_ASSIGN);
    expr = parseExpr();
    // PUNCTUATION(';')
    match(Token::PUNCTUATIONAL, ";");
    return new shortdecl(id,expr);
}

// AssignStmt -> ASSIGN Expr SEMICOLON
assign * Parser::parseAssignStmt(const std::string id){
    Expr * expr;
    // ASSIGN
    match(Token::ASSIGN);
    // Expr
    expr = parseExpr();
    // SEMICOLON
    match(Token::PUNCTUATIONAL, ";");
    return new assign(id,expr);
}

// CallStmt -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')') SEMICOLON
call * Parser::parseCallStmt(const std::string id){
    std::vector<arg*> args;
    // PUNCTUATION('(')
    match(Token::PUNCTUATIONAL, "(");
    // (ArgList)?
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        args = parseArgList();
    }
    // PUNCTUATION(')')
    match(Token::PUNCTUATIONAL, ")");
    // SEMICOLON
    match(Token::PUNCTUATIONAL,";");
    return new call(id,args);
}

// ifStmt -> KEYWORD('if') Expr Block (KEYWORD('else')(ifStmt | Block))? 
ifstmt * Parser::parseIfStmt(const bool text){
    Expr * cond = nullptr;
    Block * block = nullptr;
    ifstmt * elseptr = nullptr; 
    match(Token::KEYWORD, "if", text, "parseIfStmt");
    cond = parseExpr();
    block = parseBlock();
    if(equal(Token::KEYWORD, "else")){
        consume();
        if(equal(Token::KEYWORD, "if")){
            elseptr = parseIfStmt();
        } else {
            Block * otherblock = parseBlock();
            elseptr = new ifstmt(nullptr, otherblock, nullptr);
        }
    }

    return new ifstmt(cond, block, elseptr);
};
// forStmt -> KEYWORD('for') Expr Block
forstmt* Parser::parseForStmt(const bool text){
    Expr * cond;
    Block * block;
    match(Token::KEYWORD, "for", text, "parseForStmt");
    cond = parseExpr(text);
    block = parseBlock(text);

    return new forstmt(cond, block);
};
// returnStmt -> KEYWORD('return') (Expr)? SEMICOLON
returnstmt* Parser::parseReturnStmt(const bool text){
    Expr * expr;
    match(Token::KEYWORD, "return", text, "parseReturnStmt");
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        expr = parseExpr(text);
    }
    match(Token::PUNCTUATIONAL,";", text, "parseReturnStmt");
    return new returnstmt(expr);
};
// printStmt -> (KEYWORD('print') | KEYWORD('println')) PUNCTUATION('(') PrintArg (PUNCTUATION(',') PrintArg)* PUNCTUATION(')') SEMICOLON
printstmt* Parser::parsePrintStmt(const bool text){
    bool withEnter = false;
    std::vector<printarg*> args;
    // (KEYWORD('print') | KEYWORD('println'))
    if(equal(Token::KEYWORD,"print") || equal(Token::KEYWORD,"println")){
        withEnter = equal(Token::KEYWORD,"println");
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parsePrintStmt: expected 'print' or 'println', but got: {}", current.txt));
    }

    // PUNCTUATION('(')
    match(Token::PUNCTUATIONAL, "(", text, "parsePrintStmt");
    // PrintArg
    args.push_back(parsePrintArg(text));
    // (PUNCTUATION(',') PrintArg)*
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        args.push_back(parsePrintArg(text));
    }
    // PUNCTUATION(')')
    match(Token::PUNCTUATIONAL, ")", text, "parsePrintStmt");
    // SEMICOLON
    match(Token::PUNCTUATIONAL,";", text, "parsePrintStmt");
    return new printstmt(args, withEnter);
};
// PrintArg -> Expr | STRING
printarg* Parser::parsePrintArg(const bool text){
    Expr * expr=nullptr;
    if(equal(Token::STRING)){
        expr = new primary(current.txt, Type::STRING);
        consume(text);
    } else {
        expr = parseExpr(text);
    }

    return new printarg(expr);
};
// ArgList -> Arg (PUNCTUATION(',') Arg)*
std::vector<arg*> Parser::parseArgList(const bool text){
    std::vector<arg*> args;
    args.push_back(parseArg(text));
    while(equal(Token::PUNCTUATIONAL, ",")){
        consume(text);
        args.push_back(parseArg(text));
    }

    return args;
};

// Arg -> (ADDRESS)? Expr
arg * Parser::parseArg(const bool text){
    Expr * expr;
    bool hasAddress;
    // (ADDRESS)?
    if(equal(Token::ADDRESS)){
        hasAddress = true;
        consume(text);
    }
    // Expr
    expr = parseExpr(text);
    return new arg(expr, hasAddress);
}

// Expr -> OrExpr
Expr * Parser::parseExpr(const bool text){
    return parseOrExpr(text);
}

// OrExpr -> AndExpr (LOGICAL('||') andExpr)*
binaryexpr* Parser::parseOrExpr(const bool text){
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr * right= nullptr;
    // AndExpr
    Expr * left = parseAndExpr(text);
    // (LOGICAL('||') andExpr)*
    while(equal(Token::LOGICAL, "||")){
        op = RELATIONAL::OR;
        consume(text);
        right = parseAndExpr(text);
    }

    return new binaryexpr(op, left, right);
}

// AndExpr -> NotExpr (LOGICAL('new new ') NotExpr)*
binaryexpr* Parser::parseAndExpr(const bool text){
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr * right= nullptr;
    Expr * left = parseNotExpr(text);
    // (LOGICAL('new new ') NotExpr)*
    while(equal(Token::LOGICAL, "new new ")){
        op = RELATIONAL::AND;
        consume(text);
        right = parseNotExpr(text);
    }
    return new binaryexpr(op, left, right);
};

// NotExpr -> LOGICAL('!') NotExpr | RelExpr
unaryexpr * Parser::parseNotExpr(const bool text){
    UNARY op = UNARY::EMPTY;
    Expr * expr;
    if(equal(Token::LOGICAL, "!")){
        op = UNARY::NOT;
        consume(text);
        expr = parseNotExpr(text);
    } else {
        expr = parseRelExpr(text);
    }

    return new unaryexpr(op, expr);
};

// RelExpr -> AddExpr ((RelOp) AddExpr)*
binaryexpr * Parser::parseRelExpr(const bool text){
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr * left;
    Expr * right = nullptr;
    // AddExpr
    left = parseAddExpr(text);
    // ((RelOp) AddExpr)*
    while(equal(Token::RELATIONAL)){
        op = parseRelOp(text);
        right = parseAddExpr(text);
    }
    return new binaryexpr(op, left, right);
};

// RelOp -> RELATIONAL(any)
RELATIONAL Parser::parseRelOp(const bool text){
    std::string op = current.txt;
    match(Token::RELATIONAL);
    return toRelational(op);
};

// AddExpr -> MulExpr ((ARITH('+')| ARITH('-')) MulExpr)*
binaryexpr * Parser::parseAddExpr(const bool text){
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr * left;
    Expr * right = nullptr;
    // MulExpr
    left = parseMulExpr(text);
    // ((ARITH('+')| ARITH('-')) MulExpr)*
    while(equal(Token::ARITH, "+") || equal(Token::ARITH, "-")){
        op = toRelational(current.txt);
        consume(text);
        right = parseMulExpr(text);
    }
    return new binaryexpr(op, left, right);
};

// MulExpr -> UnaryExpr ((ARITH('*')| ARITH('/')| ARITH('%')) UnaryExpr)*
binaryexpr * Parser::parseMulExpr(const bool text){
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr * left;
    Expr * right = nullptr;
    // UnaryExpr
    left = parseUnaryExpr(text);
    // ((ARITH('*')| ARITH('/')| ARITH('%')) UnaryExpr)*
    while(equal(Token::ARITH,"*") || equal(Token::ARITH,"/") || equal(Token::ARITH,"%")){
        op = toRelational(current.txt);
        consume(text);
        right = parseUnaryExpr(text);
    }
    return new binaryexpr(op, left, right);
};

// UnaryExpr -> ARITH('-') UnaryExpr | Primary
unaryexpr * Parser::parseUnaryExpr(const bool text){
    UNARY op = UNARY::EMPTY;
    Expr * expr;
    if(equal(Token::ARITH, "-")){
        op = UNARY::MINUS;
        consume(text);
        expr = parseUnaryExpr(text);
    } else {
        expr =parsePrimary(text);
    }

    return new unaryexpr(op, expr);
};

// Primary -> INT | KEYWORD('true') | KEYWORD('false') | ID (CallExpr)? | PUNCTUATION('(') Expr PUNCTUATION(')')
Expr* Parser::parsePrimary(const bool text){
    Type type;
    PrimaryValue value;
    if(equal(Token::INT)){
        type = Type::INT;
        value = std::stoi(current.txt);
        consume(text);
    } else if(equal(Token::KEYWORD, "true")){
        type = Type::BOOL;
        value = true;
        consume(text);
    } else if(equal(Token::KEYWORD, "false")){
        type = Type::BOOL;
        value = false;
        consume(text);
    } else if(equal(Token::ID)){
        std::string id = current.txt;
        consume(text);
        if(equal(Token::PUNCTUATIONAL,"(")){
            return parseCallExpr(id);
        }
        return new primary(value, Type::ID);
    } else if(equal(Token::PUNCTUATIONAL, "(")){
        consume(text);
        return parseExpr(text);
        match(Token::PUNCTUATIONAL, ")", text);
    }
    
    throw std::runtime_error(std::format("Unexpected token in primary expression: {}", current.txt));
};

// CallExpr -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')')
call* Parser::parseCallExpr(const std::string id){
    std::vector<arg*> args;
    match(Token::PUNCTUATIONAL,"(");
    if(equal(Token::ADDRESS) || equal(Token::LOGICAL,"!") || equal(Token::ARITH,"-") || 
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")
    ){
        args = parseArgList();
    }
    match(Token::PUNCTUATIONAL, ")");

    return new call(id, args);
};  