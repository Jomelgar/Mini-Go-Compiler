#pragma once
#include<unordered_map>
#include <variant>
#include<vector>
#include<string>
#include <cstdint>

#include"./enums/type.hpp"
#include "./enums/NodeKind.hpp"

using Value = std::variant<int64_t,bool>;

struct Visitor;

struct ASTNode{
    Type * resolvedType = nullptr;
    virtual ~ASTNode() = default; 
    virtual NodeKind kind() const { return NodeKind::ASTNODE;};
    virtual void accept(Visitor& v) = 0;
};

struct ProgramNode : ASTNode{
    std::vector<ASTNode*> nodes;
    NodeKind kind() const override {
        return NodeKind::PROGRAM;
    }

    void accept(Visitor &v) override;
};

struct VarDeclNode : ASTNode{
    std::string id;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::VARDECL;
    }

    void accept(Visitor &v) override;
};

struct FunDeclNode : ASTNode{
    std::string id;
    ASTNode * paramsList;
    ASTNode * block;
    NodeKind kind() const override {
        return NodeKind::FUNDECL;
    }

    void accept(Visitor &v) override;
};

struct ParamsListNode : ASTNode{
    std::vector<ASTNode*> param;

    NodeKind kind() const override {
        return NodeKind::PARAMSLIST;
    }

    void accept(Visitor &v) override;
};

struct ParamNode : ASTNode{
    bool isRef;
    std::string id;

    NodeKind kind() const override {
        return NodeKind::PARAM;
    }

    void accept(Visitor &v) override;
};

struct BlockNode : ASTNode{
    std::vector<ASTNode*> stmt;

    NodeKind kind() const override {
        return NodeKind::BLOCK;
    }

    void accept(Visitor &v) override;
};

struct StmtNode : ASTNode{
    ASTNode * stmt;
    NodeKind kind() const override {
        return NodeKind::STMT;
    }

    void accept(Visitor &v) override;
};

struct ShortDeclNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::SHORTDECL;
    }

    void accept(Visitor &v) override;
};

struct AssignStmtNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::ASSIGNSTMT;
    }

    void accept(Visitor &v) override;
};

struct CallStmtNode : ASTNode{
    ASTNode * argList;

    NodeKind kind() const override {
        return NodeKind::CALLSTMT;
    }

    void accept(Visitor &v) override;
};

struct IfStmtNode : ASTNode{
    ASTNode * expr;
    ASTNode * block;
    ASTNode * optional;

    NodeKind kind() const override {
        return NodeKind::IFSTMT;
    }

    void accept(Visitor &v) override;
};

struct ForStmtNode : ASTNode{
    ASTNode * expr;
    ASTNode * block;
    
    NodeKind kind() const override {
        return NodeKind::FORSTMT;
    }

    void accept(Visitor &v) override;
};

struct ReturnStmtNode : ASTNode{
    ASTNode * expr;

    NodeKind kind() const override {
        return NodeKind::RETURNSTMT;
    }

    void accept(Visitor &v) override;
};

struct PrintStmtNode : ASTNode{
    bool haveLn;
    std::vector<ASTNode*> printArgs;

    NodeKind kind() const override {
        return NodeKind::PRINTSTMT;
    }

    void accept(Visitor &v) override;
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

    void accept(Visitor &v) override;
};

struct ArgListNode : ASTNode{
    ASTNode * child;
    std::vector<ASTNode*> args;

    NodeKind kind() const override {
        return NodeKind::ARGLIST;
    }

    void accept(Visitor &v) override;
};

struct ArgNode : ASTNode{
    bool isAddress;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::ARG;
    }

    void accept(Visitor &v) override;
};

struct ExprNode : ASTNode {
    ASTNode * orExpr;
    NodeKind kind() const override {
        return NodeKind::EXPR;
    }

    void accept(Visitor &v) override;
};

struct OrExprNode : ASTNode {
    std::vector<ASTNode*> andExpr;

    NodeKind kind() const override {
        return NodeKind::OREXPR;
    }

    void accept(Visitor &v) override;
};

struct AndExprNode : ASTNode {
    std::vector<ASTNode*> NotExpr;

    NodeKind kind() const override {
        return NodeKind::ANDEXPR;
    }

    void accept(Visitor &v) override;
};

struct NotExprNode : ASTNode {
    bool isNegate;
    ASTNode * expr;
    NodeKind kind() const override {
        return NodeKind::NOTEXPR;
    }

    void accept(Visitor &v) override;
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

    void accept(Visitor &v) override;
};

struct AddExprNode : ASTNode {
    ASTNode * left;
    ASTNode * right;
    char op;
    
    NodeKind kind() const override {
        return NodeKind::ADDEXPR;
    }

    void accept(Visitor &v) override;
};

struct MulExprNode : ASTNode {
    ASTNode * left;
    ASTNode * right;
    char op;
    
    NodeKind kind() const override {
        return NodeKind::MULEXPR;
    }

    void accept(Visitor &v) override;
};

struct UnaryExprNode : ASTNode {
    bool isNeg;
    ASTNode * child;
    NodeKind kind() const override {
        return NodeKind::UNARYEXPR;
    }

    void accept(Visitor &v) override;
};

struct PrimaryNode : ASTNode {
    Value value;
    NodeKind kind() const override {
        return NodeKind::PRIMARY;
    }

    void accept(Visitor &v) override;
};