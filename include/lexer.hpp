#pragma once
#include<istream>

enum class Token{
    UNK,END_OF_FILE,ARITH,ID,KEYWORD,
    STRING,NUMBER,RELATIONAL,PUNCTUATIONAL,
    INT, ADDRESS,ASSIGN,SHORT_ASSIGN,
    BOOLEAN, LOGICAL,
}; 

struct Lexeme{
    Token type;
    std::string txt;
    void toString() const;
};

class Lexer{
private:
    std::istream& input;
    char ch;
    void consume() {ch = input.get();}
    void print(bool getText, Lexeme l){
        if(getText) l.toString();
    }
public:
    Lexer(std::istream& input_): input(input_){consume();}
    Lexeme nextToken(const bool getText = false);
};