#include <string>
#include "../include/parser.hpp"

static int indentLevel = 0;
static inline std::string indentSpaces() {
    return std::string(indentLevel * 2, ' ');
}

struct IndentGuard {
    bool active;
    IndentGuard(bool active_) : active(active_) {
        if (active) ++indentLevel;
    }
    ~IndentGuard() {
        if (active) --indentLevel;
    }
};

// Init -> Program
std::vector<ASTNode *> Parser::parse(const bool text) {
    return parseProgram(text);
}

void Parser::debugEnter(std::string_view nonterminal, const bool text) const {
    if (text) std::cout << indentSpaces() << "Entering " << nonterminal << std::endl;
}

void Parser::printCurrent(const bool text) const {
    if (text) std::cout << indentSpaces() << "Current: " << current.txt << std::endl;
}

// Program -> (VarDecl | FuncDecl)* EOF
std::vector<ASTNode *> Parser::parseProgram(const bool text) {
    std::vector<ASTNode *> program;
    while (equal(Token::KEYWORD, "var") || equal(Token::KEYWORD, "func")) {
        if (current.txt == "var") {
            program.push_back(parseVarDecl(text));
        } else {
            program.push_back(parseFuncDecl(text));
        }
    }
    match(Token::END_OF_FILE, text, "parseProgram");
    return program;
}

// VarDecl -> KEYWORD('var') ID KEYWORD('int' | 'bool') (ASSIGN Expr)? PUNCTUATION(';')
vardecl *Parser::parseVarDecl(const bool text) {
    debugEnter("parseVarDecl", text);
    IndentGuard guard(text);

    std::string id;
    Type type = Type::VOID;
    Expr *expr = nullptr;

    if (equal(Token::KEYWORD, "var")) {
        consume(text);
        // ID
        id = current.txt;
        match(Token::ID, text, "parseVarDecl");
        // KEYWORD('int' | 'bool')
        if (equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")) {
            type = equal(Token::KEYWORD, "int") ? Type::INT : Type::BOOL;
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseVarDecl: expected type 'int' or 'bool', but got: {}", current.txt));
        }
        // (ASSIGN Expr)?
        if (equal(Token::ASSIGN)) {
            consume(text);
            expr = parseExpr(text);
        }
        // PUNCTUATION(';')
        match(Token::PUNCTUATIONAL, ";", text, "parseVarDecl");
    } else {
        throw std::runtime_error(std::format("Error in parseVarDecl: expected 'var' keyword, but got: {}", current.txt));
    }

    return new vardecl(type, id, expr);
}

// FuncDecl -> KEYWORD('func') ID PUNCTUATION('(') (Paramslist)? PUNCTUATION(')') (KEYWORD('int' | 'bool'))? Block
funcdecl *Parser::parseFuncDecl(const bool text) {
    debugEnter("parseFuncDecl", text);
    IndentGuard guard(text);

    std::vector<param *> params;
    Block *block;
    std::string id;
    // FIX: captura el tipo de retorno
    Type returnType = Type::VOID;

    if (equal(Token::KEYWORD, "func")) {
        consume(text);
        // ID
        id = current.txt;
        match(Token::ID, text, "parseFuncDecl");
        // PUNCTUATION('(')
        if (equal(Token::PUNCTUATIONAL, "(")) {
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseFuncDecl: expected '(', but got: {}", current.txt));
        }
        // (Paramslist)?
        if (equal(Token::ID) || equal(Token::KEYWORD, "ref")) {
            params = parseParamsList(text);
        }
        // PUNCTUATION(')')
        if (equal(Token::PUNCTUATIONAL, ")")) {
            consume(text);
        } else {
            throw std::runtime_error(std::format("Error in parseFuncDecl: expected ')', but got: {}", current.txt));
        }
        // FIX: guarda el tipo de retorno en lugar de solo consumirlo
        if (equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")) {
            returnType = equal(Token::KEYWORD, "int") ? Type::INT : Type::BOOL;
            consume(text);
        }
        // Block
        block = parseBlock(text);
    } else {
        throw std::runtime_error(std::format("Error in parseFuncDecl: expected 'func' keyword, but got: {}", current.txt));
    }

    return new funcdecl(block, params, id, returnType);
}

// ParamsList -> Param (PUNCTUATION(',') Param)*
std::vector<param *> Parser::parseParamsList(const bool text) {
    debugEnter("parseParamsList", text);
    IndentGuard guard(text);
    std::vector<param *> params;
    params.push_back(parseParam(text));
    while (equal(Token::PUNCTUATIONAL, ",")) {
        consume(text);
        params.push_back(parseParam(text));
    }
    return params;
}

// Param -> (KEYWORD('ref'))? ID KEYWORD('int' | 'bool')
param *Parser::parseParam(const bool text) {
    debugEnter("parseParam", text);
    IndentGuard guard(text);
    Type type = Type::VOID;
    std::string id;
    bool isRef = false;

    if (equal(Token::KEYWORD, "ref")) {
        consume(text);
        isRef = true;
    }
    id = current.txt;
    match(Token::ID, text, "parseParam");
    if (equal(Token::KEYWORD, "int") || equal(Token::KEYWORD, "bool")) {
        type = equal(Token::KEYWORD, "int") ? Type::INT : Type::BOOL;
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseParam: expected type 'int' or 'bool', but got: {}", current.txt));
    }

    return new param(type, id, isRef);
}

// Block -> PUNCTUATION('{') (Stmt)* PUNCTUATION('}')
Block *Parser::parseBlock(const bool text) {
    std::vector<ASTNode *> stmts;
    match(Token::PUNCTUATIONAL, "{", text, "parseBlock");
    while (equal(Token::KEYWORD, "var") || equal(Token::ID) ||
           equal(Token::KEYWORD, "if") || equal(Token::KEYWORD, "for") ||
           equal(Token::KEYWORD, "return") || equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")) {
        stmts.push_back(parseStmt(text));
    }
    if (equal(Token::PUNCTUATIONAL, "}")) {
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parseBlock: expected '}}', but got: {}", current.txt));
    }
    return new Block(stmts);
}

// Stmt -> VarDecl | ID (ShortDecl|AssignStmt|CallStmt) | ifStmt | forStmt | returnStmt | printStmt
stmt *Parser::parseStmt(const bool text) {
    debugEnter("parseStmt", text);
    IndentGuard guard(text);

    if (equal(Token::KEYWORD, "var")) {
        return parseVarDecl(text);
    } else if (equal(Token::ID)) {
        std::string id = current.txt;
        consume(text);
        // FIX: retorna el resultado en cada rama
        if (equal(Token::SHORT_ASSIGN)) {
            return parseShortDecl(id);
        } else if (equal(Token::ASSIGN)) {
            return parseAssignStmt(id);
        } else if (equal(Token::PUNCTUATIONAL, "(")) {
            return parseCallStmt(id);
        } else {
            throw std::runtime_error(std::format("Error in parseStmt: expected ':=', '=', or '(', but got: {}", current.txt));
        }
    } else if (equal(Token::KEYWORD, "if")) {
        return parseIfStmt(text);
    } else if (equal(Token::KEYWORD, "for")) {
        return parseForStmt(text);
    } else if (equal(Token::KEYWORD, "return")) {
        return parseReturnStmt(text);
    } else if (equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")) {
        return parsePrintStmt(text);
    }

    throw std::runtime_error(std::format("Error in parseStmt: unexpected token: {}", current.txt));
}

// ShortDecl -> SHORT_ASSIGN Expr PUNCTUATION(';')
shortdecl *Parser::parseShortDecl(const std::string id) {
    Expr *expr = nullptr;
    match(Token::SHORT_ASSIGN);
    expr = parseExpr();
    match(Token::PUNCTUATIONAL, ";");
    return new shortdecl(id, expr);
}

// AssignStmt -> ASSIGN Expr PUNCTUATION(';')
assign *Parser::parseAssignStmt(const std::string id) {
    Expr *expr = nullptr;
    match(Token::ASSIGN);
    expr = parseExpr();
    match(Token::PUNCTUATIONAL, ";");
    return new assign(id, expr);
}

// CallStmt -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')') PUNCTUATION(';')
call *Parser::parseCallStmt(const std::string id) {
    std::vector<arg *> args;
    match(Token::PUNCTUATIONAL, "(");
    if (equal(Token::ADDRESS) || equal(Token::LOGICAL, "!") || equal(Token::ARITH, "-") ||
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")) {
        args = parseArgList();
    }
    match(Token::PUNCTUATIONAL, ")");
    match(Token::PUNCTUATIONAL, ";");
    return new call(id, args);
}

// ifStmt -> KEYWORD('if') Expr Block (KEYWORD('else') (ifStmt | Block))?
ifstmt *Parser::parseIfStmt(const bool text) {
    debugEnter("parseIfStmt", text);
    Expr *cond = nullptr;
    Block *block = nullptr;
    ifstmt *elseptr = nullptr;

    match(Token::KEYWORD, "if", text, "parseIfStmt");
    cond = parseExpr(text);
    block = parseBlock(text);

    if (equal(Token::KEYWORD, "else")) {
        consume(text);
        if (equal(Token::KEYWORD, "if")) {
            elseptr = parseIfStmt(text);
        } else {
            Block *otherblock = parseBlock(text);
            elseptr = new ifstmt(nullptr, otherblock, nullptr);
        }
    }

    return new ifstmt(cond, block, elseptr);
}

// forStmt -> KEYWORD('for') Expr Block
forstmt *Parser::parseForStmt(const bool text) {
    debugEnter("parseForStmt", text);
    Expr *cond = nullptr;
    Block *block = nullptr;

    match(Token::KEYWORD, "for", text, "parseForStmt");
    cond = parseExpr(text);
    block = parseBlock(text);

    return new forstmt(cond, block);
}

// returnStmt -> KEYWORD('return') (Expr)? PUNCTUATION(';')
returnstmt *Parser::parseReturnStmt(const bool text) {
    debugEnter("parseReturnStmt", text);
    // FIX: inicializa a nullptr
    Expr *expr = nullptr;

    match(Token::KEYWORD, "return", text, "parseReturnStmt");
    if (equal(Token::ADDRESS) || equal(Token::LOGICAL, "!") || equal(Token::ARITH, "-") ||
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")) {
        expr = parseExpr(text);
    }
    match(Token::PUNCTUATIONAL, ";", text, "parseReturnStmt");
    return new returnstmt(expr);
}

// printStmt -> (KEYWORD('print') | KEYWORD('println')) PUNCTUATION('(') PrintArg (',' PrintArg)* PUNCTUATION(')') PUNCTUATION(';')
printstmt *Parser::parsePrintStmt(const bool text) {
    debugEnter("parsePrintStmt", text);
    bool withEnter = false;
    std::vector<printarg *> args;

    if (equal(Token::KEYWORD, "print") || equal(Token::KEYWORD, "println")) {
        withEnter = equal(Token::KEYWORD, "println");
        consume(text);
    } else {
        throw std::runtime_error(std::format("Error in parsePrintStmt: expected 'print' or 'println', but got: {}", current.txt));
    }

    match(Token::PUNCTUATIONAL, "(", text, "parsePrintStmt");
    args.push_back(parsePrintArg(text));
    while (equal(Token::PUNCTUATIONAL, ",")) {
        consume(text);
        args.push_back(parsePrintArg(text));
    }
    match(Token::PUNCTUATIONAL, ")", text, "parsePrintStmt");
    match(Token::PUNCTUATIONAL, ";", text, "parsePrintStmt");
    return new printstmt(args, withEnter);
}

// PrintArg -> STRING | Expr
printarg *Parser::parsePrintArg(const bool text) {
    Expr *expr = nullptr;
    if (equal(Token::STRING)) {
        expr = new primary(current.txt, Type::STRING);
        consume(text);
    } else {
        expr = parseExpr(text);
    }
    return new printarg(expr);
}

// ArgList -> Arg (',' Arg)*
std::vector<arg *> Parser::parseArgList(const bool text) {
    std::vector<arg *> args;
    args.push_back(parseArg(text));
    while (equal(Token::PUNCTUATIONAL, ",")) {
        consume(text);
        args.push_back(parseArg(text));
    }
    return args;
}

// Arg -> (ADDRESS)? Expr
arg *Parser::parseArg(const bool text) {
    Expr *expr = nullptr;
    bool hasAddress = false;
    if (equal(Token::ADDRESS)) {
        hasAddress = true;
        consume(text);
    }
    expr = parseExpr(text);
    return new arg(expr, hasAddress);
}

// ─────────────────────────────────────────────
//  Expresiones
// ─────────────────────────────────────────────

// Expr -> OrExpr
Expr *Parser::parseExpr(const bool text) {
    return parseOrExpr(text);
}

// OrExpr -> AndExpr (LOGICAL('||') AndExpr)*
binaryexpr *Parser::parseOrExpr(const bool text) {
    Expr *left = parseAndExpr(text);
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr *right = nullptr;

    while (equal(Token::LOGICAL, "||")) {
        op = RELATIONAL::OR;
        consume(text);
        right = parseAndExpr(text);
        left = new binaryexpr(op, left, right);
    }

    // Si nunca hubo operador, envuelve igual para mantener tipo uniforme
    if (op == RELATIONAL::EMPTY)
        return new binaryexpr(RELATIONAL::EMPTY, left, nullptr);

    return static_cast<binaryexpr *>(left);
}

// AndExpr -> NotExpr (LOGICAL('&&') NotExpr)*
binaryexpr *Parser::parseAndExpr(const bool text) {
    Expr *left = parseNotExpr(text);
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr *right = nullptr;

    while (equal(Token::LOGICAL, "&&")) {
        op = RELATIONAL::AND;
        consume(text);
        right = parseNotExpr(text);
        left = new binaryexpr(op, left, right);
    }

    if (op == RELATIONAL::EMPTY)
        return new binaryexpr(RELATIONAL::EMPTY, left, nullptr);

    return static_cast<binaryexpr *>(left);
}

// NotExpr -> LOGICAL('!') NotExpr | RelExpr
unaryexpr *Parser::parseNotExpr(const bool text) {
    if (equal(Token::LOGICAL, "!")) {
        consume(text);
        Expr *expr = parseNotExpr(text);
        return new unaryexpr(UNARY::NOT, expr);
    }
    Expr *expr = parseRelExpr(text);
    return new unaryexpr(UNARY::EMPTY, expr);
}

// RelExpr -> AddExpr (RelOp AddExpr)*
binaryexpr *Parser::parseRelExpr(const bool text) {
    Expr *left = parseAddExpr(text);
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr *right = nullptr;

    while (equal(Token::RELATIONAL)) {
        op = parseRelOp(text);
        right = parseAddExpr(text);
        left = new binaryexpr(op, left, right);
    }

    if (op == RELATIONAL::EMPTY)
        return new binaryexpr(RELATIONAL::EMPTY, left, nullptr);

    return static_cast<binaryexpr *>(left);
}

// RelOp -> RELATIONAL(any)
RELATIONAL Parser::parseRelOp(const bool text) {
    std::string op = current.txt;
    match(Token::RELATIONAL);
    return toRelational(op);
}

// AddExpr -> MulExpr (('+' | '-') MulExpr)*
binaryexpr *Parser::parseAddExpr(const bool text) {
    Expr *left = parseMulExpr(text);
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr *right = nullptr;

    while (equal(Token::ARITH, "+") || equal(Token::ARITH, "-")) {
        op = toRelational(current.txt);
        consume(text);
        right = parseMulExpr(text);
        left = new binaryexpr(op, left, right);
    }

    if (op == RELATIONAL::EMPTY)
        return new binaryexpr(RELATIONAL::EMPTY, left, nullptr);

    return static_cast<binaryexpr *>(left);
}

// MulExpr -> UnaryExpr (('*' | '/' | '%') UnaryExpr)*
binaryexpr *Parser::parseMulExpr(const bool text) {
    Expr *left = parseUnaryExpr(text);
    RELATIONAL op = RELATIONAL::EMPTY;
    Expr *right = nullptr;

    while (equal(Token::ARITH, "*") || equal(Token::ARITH, "/") || equal(Token::ARITH, "%")) {
        op = toRelational(current.txt);
        consume(text);
        right = parseUnaryExpr(text);
        left = new binaryexpr(op, left, right);
    }

    if (op == RELATIONAL::EMPTY)
        return new binaryexpr(RELATIONAL::EMPTY, left, nullptr);

    return static_cast<binaryexpr *>(left);
}

// UnaryExpr -> ARITH('-') UnaryExpr | Primary
unaryexpr *Parser::parseUnaryExpr(const bool text) {
    if (equal(Token::ARITH, "-")) {
        consume(text);
        Expr *expr = parseUnaryExpr(text);
        return new unaryexpr(UNARY::MINUS, expr);
    }
    Expr *expr = parsePrimary(text);
    return new unaryexpr(UNARY::EMPTY, expr);
}

// Primary -> INT | KEYWORD('true') | KEYWORD('false') | ID (CallExpr)? | PUNCTUATION('(') Expr PUNCTUATION(')')
Expr *Parser::parsePrimary(const bool text) {
    if (equal(Token::INT)) {
        int64_t val = std::stoll(current.txt);
        consume(text);
        return new primary(val, Type::INT);
    } else if (equal(Token::KEYWORD, "true")) {
        consume(text);
        return new primary(true, Type::BOOL);
    } else if (equal(Token::KEYWORD, "false")) {
        consume(text);
        return new primary(false, Type::BOOL);
    } else if (equal(Token::ID)) {
        // FIX: guarda el id en value para que typeCheck pueda hacer lookup
        std::string id = current.txt;
        consume(text);
        if (equal(Token::PUNCTUATIONAL, "(")) {
            // Es una llamada a función como expresión
            return parseCallExpr(id);
        }
        return new primary(id, Type::ID);
    } else if (equal(Token::PUNCTUATIONAL, "(")) {
        consume(text);
        Expr *expr = parseExpr(text);
        match(Token::PUNCTUATIONAL, ")", text, "parsePrimary");
        return expr;
    }

    throw std::runtime_error(std::format("Unexpected token in primary expression: {}", current.txt));
}

// CallExpr -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')')
call *Parser::parseCallExpr(const std::string id) {
    std::vector<arg *> args;
    // FIX: parseCallExpr es el dueño del ( y el ), no parsePrimary
    match(Token::PUNCTUATIONAL, "(");
    if (equal(Token::ADDRESS) || equal(Token::LOGICAL, "!") || equal(Token::ARITH, "-") ||
        equal(Token::ID) || equal(Token::KEYWORD, "true") || equal(Token::KEYWORD, "false") ||
        equal(Token::INT) || equal(Token::STRING) || equal(Token::PUNCTUATIONAL, "(")) {
        args = parseArgList();
    }
    match(Token::PUNCTUATIONAL, ")");
    return new call(id, args);
}