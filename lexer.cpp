#include"lexer.hpp"
#include<iostream>
#include<sstream>

bool isKeyword(const std::string& txt) {
    switch(txt.size()) {
        case 2:
            return txt == "if";
        case 3:
            return txt == "for" || txt == "var" || txt == "ref" || txt == "int" ;
        case 4:
            return txt == "else" || txt == "bool" || txt == "func" || txt == "true";
        case 5:
            return txt == "false" || txt == "print";
        case 6:
            return txt == "return";
        case 7:
            return txt == "println";
        default:
            return false;
    }
}

Lexeme Lexer::nextToken(const bool getText){
    int state = 0;
    std::ostringstream s;
    do{
        switch(state){
            case 0:
                if(ch == EOF){
                    Lexeme l = {Token::END_OF_FILE, "<<EOF>>"};
                    print(getText,l);
                    return l;
                } else if(ch == '!'){
                    state = 1;
                    s << ch;
                    consume();
                } else if (ch== '<' || ch == '>'){
                    state = 2;
                    s << ch;
                    consume();
                } else if (ch == '='){
                    state = 3;
                    s << ch;
                    consume();
                } else if (ch == '/'){
                    state = 4;
                    s << ch;
                    consume();
                } else if (ch == '\t' || ch == ' ' || ch == '\n'){
                    consume();
                    /*IGNORE IT*/
                } else if (ch == '"'){
                    state = 8;
                    s << ch;
                    consume();
                }else if (ch == '%' || (ch >= '*' && ch <= '+')){
                    s << ch;
                    consume();
                    Lexeme l = {Token::ARITH, s.str()};
                    print(getText,l);
                    return l;
                } else if(ch == '&'){
                    state = 10;
                    s << ch;
                    consume();
                } else if(ch == '|'){
                    state = 11;
                    s << ch;
                    consume();
                } else if(ch == ';' || ch == ',' || 
                    ch == '(' || ch == ')' || ch == '{' || ch == '}'){
                    s << ch;
                    consume();
                    Lexeme l = {Token::PUNCTUATIONAL, s.str()};
                    print(getText,l);
                    return l;
                } else if(ch == '-') {
                    s << ch;
                    consume();
                    state = 12;
                } else if(ch >= '0' && ch <= '9'){
                    s << ch;
                    consume();
                    state = 13;
                } else if(ch == ':'){
                    s <<ch;
                    consume();
                    state = 14;
                } else if((ch >= 'A' && ch <= 'Z') || 
                    (ch >= 'a' && ch <= 'z') || ch == '_'){
                    s << ch;
                    consume();
                    state = 15;
                } else {
                    s<<ch;
                    consume();
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 1:
                if(ch == '='){
                    s<<ch;
                    consume();
                    Lexeme l = {Token::RELATIONAL, s.str()};
                    print(getText,l);
                    return l;
                }else {
                    Lexeme l = {Token::LOGICAL, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 2:
                if(ch == '='){
                    s<<ch;
                    consume();
                    Lexeme l = {Token::RELATIONAL, s.str()};
                    print(getText,l);
                    return l;
                }else {
                    Lexeme l = {Token::RELATIONAL, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 3:
                if(ch == '='){
                    s<<ch;
                    consume();
                    Lexeme l = {Token::RELATIONAL, s.str()};
                    print(getText,l);
                    return l;
                }else {
                    Lexeme l = {Token::ASSIGN, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 4:
                if(ch == '/'){
                    s << ch;
                    consume();
                    state = 5;
                }else if(ch == '*'){
                    s << ch;
                    consume();
                    state = 6;

                } else {
                    Lexeme l = {Token::ARITH, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 5:
                if(ch == '\n' || ch == EOF){
                    s.str("");
                    consume();
                    state = 0;
                }else {
                    s << ch;
                    consume();
                }
            break;
            case 6:
                if(ch =='*'){
                    state = 7;
                    s << ch;
                    consume();

                } else if(ch == EOF){
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    s << ch;
                    consume();
                }
            break;
            case 7:
                if(ch == '/'){
                    s.str("");
                    consume();
                    state = 0;
                } else if(ch == '*'){
                    s << ch;
                    consume();
                } else if(ch == EOF){
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    s << ch;
                    state = 6;
                    consume();
                }
            break;
            case 8:
                if(ch == '"'){
                    s << ch;
                    consume();
                    Lexeme l = {Token::STRING, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    s << ch;
                    consume();
                    state = 9;
                }
            break;
            case  9:
                if(ch == '"'){
                    s << ch;
                    consume();
                    Lexeme l = {Token::STRING, s.str()};
                    print(getText,l);
                    return l;
                } else if(ch == EOF){
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    s << ch;
                    consume();
                }
            break;
            case 10:
                if(ch == '&'){
                    s << ch;
                    consume();
                    Lexeme l = {Token::LOGICAL, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    Lexeme l = {Token::ADDRESS, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 11:
                if(ch == '|'){
                    s << ch;
                    consume();
                    Lexeme l = {Token::LOGICAL, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 12:
                if(ch >= '0' && ch <= '9'){
                    s << ch;
                    consume();
                    state = 13;
                } else {
                    Lexeme l = {Token::ARITH, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 13:
                if(ch >= '0' && ch <= '9'){
                    s << ch;
                    consume();
                } else {
                    Lexeme l = {Token::INT, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 14:
                if(ch == '='){
                    s << ch;
                    consume();
                    Lexeme l = {Token::SHORT_ASSIGN, s.str()};
                    print(getText,l);
                    return l;
                } else {
                    Lexeme l = {Token::UNK, s.str()};
                    print(getText,l);
                    return l;
                }
            break;
            case 15:
                if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || 
                    (ch >= '0' && ch <= '9') || ch == '_'){
                    s << ch;
                    consume();
                } else {
                    const std::string txt = s.str();
                    Lexeme l = {isKeyword(txt) ? Token::KEYWORD : Token::ID, txt};
                    print(getText,l);
                    return l;
                }
            break;
        };
    }while(true);

};

void Lexeme::toString() const {
    switch(type){
        case Token::UNK: std::cout << "UNK: " << txt << std::endl; break;
        case Token::END_OF_FILE: std::cout << "EOF: " << txt << std::endl; break;
        case Token::ARITH: std::cout << "ARITH: " << txt << std::endl; break;
        case Token::ID: std::cout << "ID: " << txt << std::endl; break;
        case Token::KEYWORD: std::cout << "KEYWORD: " << txt << std::endl; break;
        case Token::STRING: std::cout << "STRING: " << txt << std::endl; break;
        case Token::NUMBER: std::cout << "NUMBER: " << txt << std::endl; break;
        case Token::RELATIONAL: std::cout << "RELATIONAL: " << txt << std::endl; break;
        case Token::PUNCTUATIONAL: std::cout << "PUNCTUATIONAL: " << txt << std::endl; break;
        case Token::INT: std::cout << "INT: " << txt << std::endl; break;
        case Token::ADDRESS: std::cout << "ADRESS: " << txt << std::endl; break;
        case Token::SEMICOLON: std::cout << "SEMICOLON: " << txt << std::endl; break;
        case Token::ASSIGN: std::cout << "ASSIGN: " << txt << std::endl; break;
        case Token::SHORT_ASSIGN: std::cout << "SHORT_ASSIGN: " << txt << std::endl; break;
        case Token::BOOLEAN: std::cout << "BOOLEAN: " << txt << std::endl; break;
        case Token::LOGICAL: std::cout << "LOGICAL: " << txt << std::endl; break;
    }
};
