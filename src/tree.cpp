#include"../include/tree.hpp"
#include "../include/visitor.hpp"

void ProgramNode::accept(Visitor &v) { v.visit(*this); }

void VarDeclNode::accept(Visitor &v) { v.visit(*this); }

void FunDeclNode::accept(Visitor &v) { v.visit(*this); }

void ParamsListNode::accept(Visitor &v) { v.visit(*this); }

void ParamNode::accept(Visitor &v) { v.visit(*this); }

void BlockNode::accept(Visitor &v) { v.visit(*this); }

void StmtNode::accept(Visitor &v) { v.visit(*this); }

void ShortDeclNode::accept(Visitor &v) { v.visit(*this); }

void AssignStmtNode::accept(Visitor &v) { v.visit(*this); }

void CallStmtNode::accept(Visitor &v) { v.visit(*this); }

void IfStmtNode::accept(Visitor &v) { v.visit(*this); }

void ForStmtNode::accept(Visitor &v) { v.visit(*this); }

void ReturnStmtNode::accept(Visitor &v) { v.visit(*this); }

void PrintStmtNode::accept(Visitor &v) { v.visit(*this); }

void PrintArgNode::accept(Visitor &v) { v.visit(*this); }

void ArgListNode::accept(Visitor &v) { v.visit(*this); }

void ArgNode::accept(Visitor &v) { v.visit(*this); }

void ExprNode::accept(Visitor &v) { v.visit(*this); }

void OrExprNode::accept(Visitor &v) { v.visit(*this); }

void AndExprNode::accept(Visitor &v) { v.visit(*this); }

void NotExprNode::accept(Visitor &v) { v.visit(*this); }

void RelExprNode::accept(Visitor &v) { v.visit(*this); }

void AddExprNode::accept(Visitor &v) { v.visit(*this); }

void MulExprNode::accept(Visitor &v) { v.visit(*this); }

void UnaryExprNode::accept(Visitor &v) { v.visit(*this); }

void PrimaryNode::accept(Visitor &v) { v.visit(*this); }

