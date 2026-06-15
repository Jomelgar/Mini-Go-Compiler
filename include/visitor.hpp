#pragma once

#include<map>
#include<string>
#include "tree.hpp"

using Scope = std::map<std::string,Value>;
using Mult_Scope = std::vector<Scope>;

struct Visitor {
    virtual ~Visitor() = default;

    virtual void visit(ProgramNode& n) = 0;
    virtual void visit(VarDeclNode& n) = 0;
    virtual void visit(FunDeclNode& n) = 0;
    virtual void visit(ParamsListNode& n) = 0;
    virtual void visit(ParamNode& n) = 0;
    virtual void visit(BlockNode& n) = 0;

    virtual void visit(StmtNode& n) = 0;
    virtual void visit(ShortDeclNode& n) = 0;
    virtual void visit(AssignStmtNode& n) = 0;
    virtual void visit(CallStmtNode& n) = 0;
    virtual void visit(IfStmtNode& n) = 0;
    virtual void visit(ForStmtNode& n) = 0;
    virtual void visit(ReturnStmtNode& n) = 0;
    virtual void visit(PrintStmtNode& n) = 0;
    virtual void visit(PrintArgNode& n) = 0;

    virtual void visit(ArgListNode& n) = 0;
    virtual void visit(ArgNode& n) = 0;

    virtual void visit(ExprNode& n) = 0;
    virtual void visit(OrExprNode& n) = 0;
    virtual void visit(AndExprNode& n) = 0;
    virtual void visit(NotExprNode& n) = 0;
    virtual void visit(RelExprNode& n) = 0;
    virtual void visit(AddExprNode& n) = 0;
    virtual void visit(MulExprNode& n) = 0;
    virtual void visit(UnaryExprNode& n) = 0;
    virtual void visit(PrimaryNode& n) = 0;
};

struct TypeChecker : Visitor {
    Mult_Scope symbols;

    void visit(ProgramNode& n)  override;
    void visit(VarDeclNode& n) override;
    void visit(FunDeclNode& n) override;
    void visit(ParamsListNode& n)  override;
    void visit(ParamNode& n) override;
    void visit(BlockNode& n) override;
    void visit(StmtNode& n) override;
    void visit(ShortDeclNode& n) override;
    void visit(AssignStmtNode& n) override;
    void visit(CallStmtNode& n)  override;
    void visit(IfStmtNode& n) override;
    void visit(ForStmtNode& n)  override;
    void visit(ReturnStmtNode& n)  override;
    void visit(PrintStmtNode& n) override;
    void visit(PrintArgNode& n)  override;
    void visit(ArgListNode& n) override;
    void visit(ArgNode& n)  override;
    void visit(ExprNode& n)  override;
    void visit(OrExprNode& n)  override;
    void visit(AndExprNode& n) override;
    void visit(NotExprNode& n) override;
    void visit(RelExprNode& n) override;
    void visit(AddExprNode& n) override;
    void visit(MulExprNode& n) override;
    void visit(UnaryExprNode& n) override;
    void visit(PrimaryNode& n) override;
};

struct Interpret : Visitor {
    void visit(ProgramNode& n)  override;
    void visit(VarDeclNode& n) override;
    void visit(FunDeclNode& n) override;
    void visit(ParamsListNode& n)  override;
    void visit(ParamNode& n) override;
    void visit(BlockNode& n) override;
    void visit(StmtNode& n) override;
    void visit(ShortDeclNode& n) override;
    void visit(AssignStmtNode& n) override;
    void visit(CallStmtNode& n)  override;
    void visit(IfStmtNode& n) override;
    void visit(ForStmtNode& n)  override;
    void visit(ReturnStmtNode& n)  override;
    void visit(PrintStmtNode& n) override;
    void visit(PrintArgNode& n)  override;
    void visit(ArgListNode& n) override;
    void visit(ArgNode& n)  override;
    void visit(ExprNode& n)  override;
    void visit(OrExprNode& n)  override;
    void visit(AndExprNode& n) override;
    void visit(NotExprNode& n) override;
    void visit(RelExprNode& n) override;
    void visit(AddExprNode& n) override;
    void visit(MulExprNode& n) override;
    void visit(UnaryExprNode& n) override;
    void visit(PrimaryNode& n) override;
};