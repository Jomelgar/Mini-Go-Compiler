#pragma once
#include<format>
#include "lexer.hpp"

class Parser {
private:
    Lexer& lexer;
    Lexeme current;
    void consume(const bool text = false) {current = lexer.nextToken(text);}
    void match(Token expected){
        if(current.type == expected) {
            consume();
        } else{
            throw std::runtime_error(std::format("Expected token: {}, but got: {}",static_cast<int>(expected), static_cast<int>(current.type)));
        }
    };
public:
    Parser(Lexer& lexer_) : lexer(lexer_) {consume();} 
    void parse(const bool text = false);
private:
    // Another parsing functions

};