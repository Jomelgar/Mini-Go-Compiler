#pragma once

enum class Kind {
    PARAM,
    BLOCK,
    PRINTARG,
    ARG,
//STMT,
    VARDECL,
    FUNDECL,
    SHORTDECL,
    ASSIGN,
    CALL,
    IF,
    FOR,
    RETURN,
    PRINT,
// ARITH && LOG
    BINARY_EXPR,
// UNARY_EXPR    
    UNARYEXPR,
// PRIMARY
    PRIMARY,
};
