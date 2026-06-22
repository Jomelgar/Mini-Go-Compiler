#pragma once
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <format>
#include "./enums/type.hpp"
#include "./enums/NodeKind.hpp"
#include "./enums/relational.hpp"
#include "./enums/unary.hpp"

using PrimaryValue = std::variant<int64_t, bool, std::string>;
class param;

struct VarInfo {
    Type type;
    bool isRef = false;
};

struct FuncInfo {
    Type returnType = Type::VOID;
    std::vector<param*> params;
};

using TableValue = std::variant<VarInfo, FuncInfo>;

struct Scope {
    std::vector<std::unordered_map<std::string, TableValue>> scopes;

    void push() {
        scopes.push_back({});
    }

    void pop() {
        scopes.pop_back();
    }

    bool isGlobal() const { return scopes.size() == 1; }

    void declare(const std::string& id, TableValue val) {
        if (scopes.back().count(id))
            throw std::runtime_error(std::format("'{}' ya fue declarado en este scope", id));
        scopes.back()[id] = val;
    }

    TableValue* lookup(const std::string& id) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
            if (it->count(id)) return &(*it)[id];
        return nullptr;
    }

    Type currentReturnType() {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->count("0return")) {
                return std::get<VarInfo>((*it)["0return"]).type;
            }
        }
        return Type::VOID;
    }

    void setReturnType(Type t) {
        scopes.back()["0return"] = VarInfo{t, false};
    }
};



class ASTNode {
public:
    Kind kind;
    ASTNode(Kind k) : kind(k) {}
    virtual ~ASTNode() = default;
    virtual void typeCheck(Scope& scope) = 0;
};

class stmt : public ASTNode {
public:
    stmt(Kind k) : ASTNode(k) {}
};

class Expr : public ASTNode {
public:
    Type type = Type::VOID;
    Expr(Kind k) : ASTNode(k) {}
};

class Block : public ASTNode {
public:
    std::vector<ASTNode*> stmts;
    Block(std::vector<ASTNode*> stmts_) : ASTNode(Kind::BLOCK), stmts(stmts_) {}
    void typeCheck(Scope& scope) override;
};

class param : public ASTNode {
public:
    Type type;
    std::string id;
    bool isRef;
    param(Type type_, std::string id_, bool isRef_)
        : ASTNode(Kind::PARAM), type(type_), id(id_), isRef(isRef_) {}
    void typeCheck(Scope& scope) override;
};

class arg : public ASTNode {
public:
    Expr* expr;
    bool hasAddress;
    arg(Expr* expr_, bool hasAddress_)
        : ASTNode(Kind::ARG), expr(expr_), hasAddress(hasAddress_) {}
    void typeCheck(Scope& scope) override;
};

class printarg : public ASTNode {
public:
    Expr* expr;
    printarg(Expr* expr_) : ASTNode(Kind::PRINTARG), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class funcdecl : public stmt {
public:
    std::vector<param*> params;
    Block* block;
    std::string id;
    Type returnType;
    funcdecl(Block* block_, std::vector<param*> params_, std::string id_, Type ret_ = Type::VOID)
        : stmt(Kind::FUNDECL), block(block_), params(params_), id(id_), returnType(ret_) {}
    void typeCheck(Scope& scope) override;
};

class vardecl : public stmt {
public:
    Type type;
    std::string id;
    Expr* expr;
    vardecl(Type type_, std::string id_, Expr* expr_)
        : stmt(Kind::VARDECL), type(type_), id(id_), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class shortdecl : public stmt {
public:
    std::string id;
    Expr* expr;
    shortdecl(std::string id_, Expr* expr_)
        : stmt(Kind::SHORTDECL), id(id_), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class assign : public stmt {
public:
    std::string id;
    Expr* expr;
    assign(std::string id_, Expr* expr_)
        : stmt(Kind::ASSIGN), id(id_), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class call : public Expr, public stmt {
public:
    std::string id;
    std::vector<arg*> args;
    call(std::string id_, std::vector<arg*> args_)
        : Expr(Kind::CALL), stmt(Kind::CALL), id(id_), args(args_) {}
    void typeCheck(Scope& scope) override;
};

class ifstmt : public stmt {
public:
    Expr* cond;
    Block* block;
    ifstmt* elseptr;
    ifstmt(Expr* cond_, Block* block_, ifstmt* elseptr_)
        : stmt(Kind::IF), cond(cond_), block(block_), elseptr(elseptr_) {}
    void typeCheck(Scope& scope) override;
};

class forstmt : public stmt {
public:
    Expr* cond;
    Block* block;
    forstmt(Expr* cond_, Block* block_)
        : stmt(Kind::FOR), cond(cond_), block(block_) {}
    void typeCheck(Scope& scope) override;
};

class returnstmt : public stmt {
public:
    Expr* expr;  // puede ser nullptr si la func es void
    returnstmt(Expr* expr_) : stmt(Kind::RETURN), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class printstmt : public stmt {
public:
    bool withEnter;
    std::vector<printarg*> args;
    printstmt(std::vector<printarg*> args_, bool withEnter_ = false)
        : stmt(Kind::PRINT), args(args_), withEnter(withEnter_) {}
    void typeCheck(Scope& scope) override;
};

class binaryexpr : public Expr {
public:
    RELATIONAL op;
    Expr* left;
    Expr* right;
    binaryexpr(RELATIONAL op_, Expr* left_, Expr* right_)
        : Expr(Kind::BINARY_EXPR), op(op_), left(left_), right(right_) {}
    void typeCheck(Scope& scope) override;
};

class unaryexpr : public Expr {
public:
    UNARY op;
    Expr* expr;
    unaryexpr(UNARY op_, Expr* expr_)
        : Expr(Kind::UNARYEXPR), op(op_), expr(expr_) {}
    void typeCheck(Scope& scope) override;
};

class primary : public Expr {
public:
    PrimaryValue value;
    primary(PrimaryValue value_, Type type_)
        : Expr(Kind::PRIMARY), value(value_) { this->type = type_; }
    void typeCheck(Scope& scope) override;
};