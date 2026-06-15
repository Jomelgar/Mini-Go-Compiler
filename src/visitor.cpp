#include"../include/visitor.hpp"

/* ---------------------- TypeChecker ---------------------- */
void TypeChecker::visit(ProgramNode& n) {
    for (auto node : n.nodes){
        node->accept(*this);
    }

}

void TypeChecker::visit(VarDeclNode& n) { 

}

void TypeChecker::visit(FunDeclNode& n) { 

}

void TypeChecker::visit(ParamsListNode& n)  { 

}

void TypeChecker::visit(ParamNode& n) { 

}

void TypeChecker::visit(BlockNode& n) { 

}

void TypeChecker::visit(StmtNode& n) { 

}

void TypeChecker::visit(ShortDeclNode& n) { 

}

void TypeChecker::visit(AssignStmtNode& n) { 

}

void TypeChecker::visit(CallStmtNode& n)  { 

}

void TypeChecker::visit(IfStmtNode& n) { 

}

void TypeChecker::visit(ForStmtNode& n)  { 

}

void TypeChecker::visit(ReturnStmtNode& n)  { 

}

void TypeChecker::visit(PrintStmtNode& n) { 

}

void TypeChecker::visit(PrintArgNode& n)  { 

}

void TypeChecker::visit(ArgListNode& n) { 

}

void TypeChecker::visit(ArgNode& n)  { 

}

void TypeChecker::visit(ExprNode& n)  { 

}

void TypeChecker::visit(OrExprNode& n)  { 

}

void TypeChecker::visit(AndExprNode& n) { 

}

void TypeChecker::visit(NotExprNode& n) { 

}

void TypeChecker::visit(RelExprNode& n) { 

}

void TypeChecker::visit(AddExprNode& n) { 

}

void TypeChecker::visit(MulExprNode& n) { 

}

void TypeChecker::visit(UnaryExprNode& n) { 

}

void TypeChecker::visit(PrimaryNode& n) { 

}
