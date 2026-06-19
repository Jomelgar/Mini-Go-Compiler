#pragma once
#include <string>
#include <stdexcept>


enum class RELATIONAL{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    AND,
    OR,
    LT,
    LE,
    GE,
    GT,
    E,
    NE,
    EMPTY
};

inline RELATIONAL toRelational(const std::string& op) {
    if (op == "<") {
        return RELATIONAL::LT;
    } 
    else if (op == ">") {
        return RELATIONAL::GT;
    } 
    else if (op == "<=") {
        return RELATIONAL::LE;
    } 
    else if (op == ">=") {
        return RELATIONAL::GE;
    } 
    else if (op == "==") {
        return RELATIONAL::E;
    } 
    else if (op == "!=") {
        return RELATIONAL::NE;
    } 
    else if (op == "+") {
        return RELATIONAL::ADD;
    } 
    else if (op == "-") {
        return RELATIONAL::SUB;
    } 
    else if (op == "*") {
        return RELATIONAL::MUL;
    } 
    else if (op == "/") {
        return RELATIONAL::DIV;
    }else if(op == "%"){
        return RELATIONAL::MOD;    
    }

    throw std::runtime_error("Unknown relational/operator: " + op);
};