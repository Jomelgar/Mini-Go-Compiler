#pragma once
#include <format>
#include <iostream>
#include <string_view>
#include "tree.hpp"
#include "lexer.hpp"

/*
* Soy consciente que algunos lados de esta gramatica les tuve que crear 
* nuevos no terminales sin embargo se me olvido que aunque factorice lo que no era igual
* se debe pasar a un nuevo no terminal. Asi que:
* TODO: En un futuro realizar en otros no terminales lo que no tiene común algo no factorizado.
* Ej: ID KEYWORD('int' | 'bool') -> ID TYPE; TYPE = int | bool 
*/

class Parser {
private:
    Lexer& lexer;
    Lexeme current;
    void consume(const bool text = false) { if(text) printCurrent(text); current = lexer.nextToken(); }
    bool equal(Token expected) const {return current.type == expected;}
    void debugEnter(std::string_view nonterminal, bool text) const;
    void printCurrent(const bool text) const;
    void match(Token expected, const bool text = false, std::string_view nonterminal = "parser"){
        if(current.type == expected) {
            consume(text);
        } else{
            throw std::runtime_error(std::format("Error in {}: expected token {}, but got: {}", nonterminal, static_cast<int>(expected), current.txt));
        }
    };
    void match(Token expected, std::string expectedTxt,const bool text = false, std::string_view nonterminal = "parser"){
        if(current.type == expected && current.txt == expectedTxt) {
            consume(text);
        } else{
            throw std::runtime_error(std::format("Error in {}: expected token {}, but got: {}", nonterminal, expectedTxt, current.txt));
        }
    };
    bool equal(Token expected, const std::string& expectedTxt) const{
        return (current.type == expected && current.txt == expectedTxt);
    };
public:
    Parser(Lexer& lexer_) : lexer(lexer_) {consume();} 
    void parse(const bool text = false);
private:
// -------------------------------------------------
    // Program -> (VarDecl | FuncDecl)* EOF
    void parseProgram(const bool text = false);

/*-------------------------DECLARATIONS-------------------------*/
    // VarDecl -> KEYWORD('var') ID KEYWORD('int' | 'bool') (ASSIGN Expr)? PUNCTUATION(';')
    void parseVarDecl(const bool text = false);
    // FuncDecl -> KEYWORD('func') ID PUNCTUATION('(') (Paramslist)? PUNCTUATION(')') (KEYWORD('int' | 'bool'))? Block   
    void parseFuncDecl(const bool text = false); 
    // ParamsList -> param (PUNCTUATION(',') param)*
    void parseParamsList(const bool text = false);
    // param -> (KEYWORD('ref'))? ID KEYWORD('int' | 'bool')
    void parseParam(const bool text = false);
/*-------------------------STATEMENTS---------------------------*/
    // Block -> PUNCTUATION('{') (Stmt)* PUNCTUATION('}')
    void parseBlock(const bool text = false);
    // Stmt -> VarDecl|ID (ShortDecl|AssignStmt|CallStmt) | ifStmt | forStmt | returnStmt | printStmt
    void parseStmt(const bool text = false);
    // ShortDecl -> SHORT_ASSIGN Expr PUNCTUATION(';')
    void parseShortDecl(const bool text = false);
    // AssignStmt -> ASSIGN Expr PUNCTUATION(';')
    void parseAssignStmt(const bool text = false);
    // CallStmt -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')') PUNCTUATION(';')
    void parseCallStmt(const bool text = false);
    // ifStmt -> KEYWORD('if') Expr Block (KEYWORD('else')(ifStmt | Block))? 
    void parseIfStmt(const bool text = false);
    // forStmt -> KEYWORD('for') Expr Block
    void parseForStmt(const bool text = false);
    // returnStmt -> KEYWORD('return') (Expr)? SEMICOLON
    void parseReturnStmt(const bool text = false);
    // printStmt -> (KEYWORD('print') | KEYWORD('println')) PUNCTUATION('(') PrintArg (PUNCTUATION(',') PrintArg)* PUNCTUATION(')') SEMICOLON
    void parsePrintStmt(const bool text = false);
    // PrintArg -> Expr | STRING
    void parsePrintArg(const bool text = false);
    // ArgList -> Arg (PUNCTUATION(',') Arg)*
    void parseArgList(const bool text = false);
    // Arg -> (ADDRESS)? Expr
    void parseArg(const bool text = false);
/*-------------------------EXPRESSIONS--------------------------*/
    // Expr -> OrExpr
    void parseExpr(const bool text = false);
    // OrExpr -> AndExpr (LOGICAL('||') andExpr)*
    void parseOrExpr(const bool text = false);
    // AndExpr -> NotExpr (LOGICAL('&&') NotExpr)*
    void parseAndExpr(const bool text = false);
    // NotExpr -> LOGICAL('!') NotExpr | RelExpr
    void parseNotExpr(const bool text = false);
    // RelExpr -> AddExpr ((RelOp) AddExpr)*
    void parseRelExpr(const bool text = false);
    // RelOp -> RELATIONAL(any)
    void parseRelOp(const bool text = false);
    // AddExpr -> MulExpr ((ARITH('+')| ARITH('-')) MulExpr)*
    void parseAddExpr(const bool text = false);
    // MulExpr -> UnaryExpr ((ARITH('*')| ARITH('/')| ARITH('%')) UnaryExpr)*
    void parseMulExpr(const bool text = false);
    // UnaryExpr -> ARITH('-') UnaryExpr | Primary
    void parseUnaryExpr(const bool text = false);
    // Primary -> INT | KEYWORD('true') | KEYWORD('false') | ID (CallExpr)? | PUNCTUATION('(') Expr PUNCTUATION(')')
    void parsePrimary(const bool text = false);
    // CallExpr -> PUNCTUATION('(') (ArgList)? PUNCTUATION(')')
    void parseCallExpr(const bool text = false);  
/*--------------------------------------------------------------*/

};