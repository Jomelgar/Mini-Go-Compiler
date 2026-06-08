#include<fstream>
#include<iostream>
#include"./include/lexer.hpp"
#include"./include/parser.hpp"

int main(int argc,char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if(!file.is_open()){
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }
    
    Lexer lexer(file);
    Parser parser(lexer);
    try {
        parser.parse();
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return  0;
};