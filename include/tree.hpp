#pragma once
#include<unordered_map>
#include <variant>
#include<vector>
#include<string>
#include <cstdint>

#include"./enums/type.hpp"
#include "./enums/NodeKind.hpp"

using Value = std::variant<int64_t,bool>;

struct ASTNode{
    Type * resolvedType = nullptr;
    virtual ~ASTNode() = default; 
    virtual NodeKind kind() const { return NodeKind::ASTNODE;};
    virtual void accept(Visitor* v) = 0;
};

struct ProgramNode : ASTNode{
    std::vector<ASTNode*> nodes;
    NodeKind kind() const override {
        return NodeKind::PROGRAM;
    }

    void accept(Visitor *v) { v->visit(*this) }
};

struct VarDeclNode : ASTNode{
    std::string id;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::VARDECL;
    }
};

struct FunDeclNode : ASTNode{
    std::string id;
    ASTNode * paramsList;
    ASTNode * block;
    NodeKind kind() const override {
        return NodeKind::FUNDECL;
    }
};

struct ParamsListNode : ASTNode{
    std::vector<ASTNode*> param;
    NodeKind kind() const override {
        return NodeKind::PARAMSLIST;
    }
};

struct ParamNode : ASTNode{
    bool isRef;
    std::string id;
    NodeKind kind() const override {
        return NodeKind::PARAM;
    }
};

struct BlockNode : ASTNode{
    std::vector<ASTNode*> stmt;

    NodeKind kind() const override {
        return NodeKind::BLOCK;
    }
};

struct StmtNode : ASTNode{
    ASTNode * stmt;
    NodeKind kind() const override {
        return NodeKind::STMT;
    }
};

struct ShortDeclNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::SHORTDECL;
    }
};

struct AssignStmtNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::ASSIGNSTMT;
    }
};

struct CallStmtNode : ASTNode{
    ASTNode * argList;

    NodeKind kind() const override {
        return NodeKind::CALLSTMT;
    }
};

struct IfStmtNode : ASTNode{
    ASTNode * expr;
    ASTNode * block;
    ASTNode * optional;

    NodeKind kind() const override {
        return NodeKind::IFSTMT;
    }
};

struct ForStmtNode : ASTNode{
    ASTNode * expr;
    ASTNode * block;
    
    NodeKind kind() const override {
        return NodeKind::FORSTMT;
    }
};

struct ReturnStmtNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::RETURNSTMT;
    }
};

struct PrintStmtNode : ASTNode{
    bool haveLn;
    std::vector<ASTNode*> printArgs;

    NodeKind kind() const override {
        return NodeKind::PRINTSTMT;
    }
};

struct PrintArgNode : ASTNode{
    enum class Kind {
        EXPR,
        STRING
    };

    Kind argKind;
    ASTNode* expr = nullptr;
    std::string value;

    NodeKind kind() const override {
        return NodeKind::PRINTARG;
    }
};

struct ArgListNode : ASTNode{
    ASTNode * child;
    std::vector<ASTNode*> args;

    NodeKind kind() const override {
        return NodeKind::ARGLIST;
    }
};

struct ArgNode : ASTNode{
    bool isAddress;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::ARG;
    }
};

struct ExprNode : ASTNode {
    ASTNode * orExpr;
    NodeKind kind() const override {
        return NodeKind::EXPR;
    }
};

struct OrExprNode : ASTNode {
    std::vector<ASTNode*> andExpr;

    NodeKind kind() const override {
        return NodeKind::OREXPR;
    }
};

struct AndExprNode : ASTNode {
    std::vector<ASTNode*> NotExpr;

    NodeKind kind() const override {
        return NodeKind::ANDEXPR;
    }
};

struct NotExprNode : ASTNode {
    bool isNegate;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::NOTEXPR;
    }
};

struct RelExprNode : ASTNode {
    enum class RelOp {
        LT, GT, LE, GE, EQ, NEQ
    };
    
    ASTNode* left;
    ASTNode* right;
    RelOp op;

    NodeKind kind() const override {
        return NodeKind::RELEXPR;
    }
};

struct AddExprNode : ASTNode {
    ASTNode * left;
    ASTNode * right;
    char op;
    
    NodeKind kind() const override {
        return NodeKind::ADDEXPR;
    }
};

struct MulExprNode : ASTNode {
    ASTNode * left;
    ASTNode * right;
    char op;
    
    NodeKind kind() const override {
        return NodeKind::MULEXPR;
    }
};

struct UnaryExprNode : ASTNode {
    bool isNeg;
    ASTNode * child;
    NodeKind kind() const override {
        return NodeKind::UNARYEXPR;
    }
};

struct PrimaryNode : ASTNode {
    Value value;
    NodeKind kind() const override {
        return NodeKind::PRIMARY;
    }
};