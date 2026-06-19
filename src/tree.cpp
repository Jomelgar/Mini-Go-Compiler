#include "../include/tree.hpp"
#include <iostream>
#include <format>

// ─────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────

static bool isArithOp(RELATIONAL op) {
    return op == RELATIONAL::ADD || op == RELATIONAL::SUB ||
           op == RELATIONAL::MUL || op == RELATIONAL::DIV ||
           op == RELATIONAL::MOD;
}

static bool isRelOp(RELATIONAL op) {
    return op == RELATIONAL::LT  || op == RELATIONAL::GT ||
           op == RELATIONAL::LE || op == RELATIONAL::GE;
}

static bool isEqOp(RELATIONAL op) {
    return op == RELATIONAL::E || op == RELATIONAL::NE;
}

static bool isLogicOp(RELATIONAL op) {
    return op == RELATIONAL::AND || op == RELATIONAL::OR;
}

static std::string typeName(Type t) {
    switch (t) {
        case Type::INT:  return "int";
        case Type::BOOL: return "bool";
        case Type::VOID: return "void";
        default:         return "unknown";
    }
}

// ─────────────────────────────────────────────
//  Block
// ─────────────────────────────────────────────

// Block abre su propio scope
void Block::typeCheck(Scope& scope) {
    scope.push();
    for (ASTNode* s : stmts)
        s->typeCheck(scope);
    scope.pop();
}

// ─────────────────────────────────────────────
//  Declaraciones
// ─────────────────────────────────────────────

void vardecl::typeCheck(Scope& scope) {
    // Verifica duplicado en scope actual
    scope.declare(id, VarInfo{type, false});

    if (expr) {
        expr->typeCheck(scope);
        if (expr->type == Type::VOID)
            throw std::runtime_error(std::format("vardecl '{}': la expresión es void", id));
        if (expr->type != type)
            throw std::runtime_error(std::format(
                "vardecl '{}': se esperaba '{}' pero la expresión es '{}'",
                id, typeName(type), typeName(expr->type)));
    }
}

void shortdecl::typeCheck(Scope& scope) {
    // := infiere el tipo de la expresión
    if (!expr)
        throw std::runtime_error(std::format("shortdecl '{}': expresión nula", id));

    expr->typeCheck(scope);

    if (expr->type == Type::VOID)
        throw std::runtime_error(std::format("shortdecl '{}': la expresión es void", id));

    // Declara la variable con el tipo inferido
    scope.declare(id, VarInfo{expr->type, false});
}

void assign::typeCheck(Scope& scope) {
    // La variable debe existir
    TableValue* val = scope.lookup(id);
    if (!val)
        throw std::runtime_error(std::format("assign: '{}' no fue declarado", id));

    if (!std::holds_alternative<VarInfo>(*val))
        throw std::runtime_error(std::format("assign: '{}' es una función, no una variable", id));

    VarInfo& info = std::get<VarInfo>(*val);

    expr->typeCheck(scope);

    if (expr->type != info.type)
        throw std::runtime_error(std::format(
            "assign '{}': se esperaba '{}' pero se asignó '{}'",
            id, typeName(info.type), typeName(expr->type)));
}

// ─────────────────────────────────────────────
//  Funciones
// ─────────────────────────────────────────────

void funcdecl::typeCheck(Scope& scope) {
    // Registra la función en el scope actual antes de entrar al bloque
    // (permite recursión)
    scope.declare(id, FuncInfo{returnType, params});

    // Scope de la función
    scope.push();
    scope.setReturnType(returnType);

    // Registra los parámetros como variables locales
    for (param* p : params)
        p->typeCheck(scope);

    // Typecheckea el cuerpo (Block NO abre otro scope aquí,
    // usamos directamente el scope de la función)
    scope.push();
    for (ASTNode* s : block->stmts)
        s->typeCheck(scope);
    scope.pop();

    scope.pop();
}

void param::typeCheck(Scope& scope) {
    scope.declare(id, VarInfo{type, isRef});
}

// ─────────────────────────────────────────────
//  Call (como stmt y como expr)
// ─────────────────────────────────────────────

void call::typeCheck(Scope& scope) {
    TableValue* val = scope.lookup(id);
    if (!val)
        throw std::runtime_error(std::format("call: '{}' no fue declarado", id));

    if (!std::holds_alternative<FuncInfo>(*val))
        throw std::runtime_error(std::format("call: '{}' no es una función", id));

    FuncInfo& info = std::get<FuncInfo>(*val);

    // Verifica cantidad de argumentos
    if (args.size() != info.params.size())
        throw std::runtime_error(std::format(
            "call '{}': se esperaban {} args pero se pasaron {}",
            id, info.params.size(), args.size()));

    // Verifica tipo de cada argumento contra el parámetro correspondiente
    for (size_t i = 0; i < args.size(); i++) {
        args[i]->typeCheck(scope);

        Type argType  = args[i]->expr->type;
        Type paramType = info.params[i]->type;
        bool paramIsRef = info.params[i]->isRef;

        if (argType != paramType)
            throw std::runtime_error(std::format(
                "call '{}' arg {}: se esperaba '{}' pero se pasó '{}'",
                id, i, typeName(paramType), typeName(argType)));

        // Si el param es ref, el arg debe ser un ID (lvalue)
        if (paramIsRef && args[i]->expr->kind != Kind::PRIMARY)
            throw std::runtime_error(std::format(
                "call '{}' arg {}: parámetro ref requiere una variable", id, i));

        if (paramIsRef && args[i]->expr->kind == Kind::PRIMARY) {
            auto* p = static_cast<primary*>(args[i]->expr);
            if (!std::holds_alternative<std::string>(p->value))
                throw std::runtime_error(std::format(
                    "call '{}' arg {}: parámetro ref requiere una variable (lvalue)", id, i));
        }
    }

    // El tipo de retorno del call es el de la función
    this->type = info.returnType;
}

// ─────────────────────────────────────────────
//  Control de flujo
// ─────────────────────────────────────────────

void ifstmt::typeCheck(Scope& scope) {
    if (cond) {
        cond->typeCheck(scope);
        if (cond->type != Type::BOOL)
            throw std::runtime_error(std::format(
                "if: la condición debe ser bool, no '{}'", typeName(cond->type)));
        block->typeCheck(scope);
    }

    if (elseptr)
        elseptr->typeCheck(scope);
}

void forstmt::typeCheck(Scope& scope) {
    cond->typeCheck(scope);
    if (cond->type != Type::BOOL)
        throw std::runtime_error(std::format(
            "for: la condición debe ser bool, no '{}'", typeName(cond->type)));
    block->typeCheck(scope);
}

void returnstmt::typeCheck(Scope& scope) {
    Type expected = scope.currentReturnType();

    if (!expr) {
        // return; solo válido en funciones void
        if (expected != Type::VOID)
            throw std::runtime_error(std::format(
                "return: la función espera '{}' pero no se retornó valor", typeName(expected)));
        return;
    }

    expr->typeCheck(scope);

    if (expr->type != expected)
        throw std::runtime_error(std::format(
            "return: se esperaba '{}' pero se retornó '{}'",
            typeName(expected), typeName(expr->type)));
}

// ─────────────────────────────────────────────
//  Print
// ─────────────────────────────────────────────

void printstmt::typeCheck(Scope& scope) {
    for (printarg* pa : args)
        pa->typeCheck(scope);
}

void printarg::typeCheck(Scope& scope) {
    // Acepta int, bool o string — solo valida que la expresión sea válida
    expr->typeCheck(scope);
    if (expr->type == Type::VOID)
        throw std::runtime_error("printarg: no se puede imprimir una expresión void");
}

// ─────────────────────────────────────────────
//  Arg
// ─────────────────────────────────────────────

void arg::typeCheck(Scope& scope) {
    expr->typeCheck(scope);
    // La validación de tipo se hace en call::typeCheck
}

// ─────────────────────────────────────────────
//  Expresiones
// ─────────────────────────────────────────────

void primary::typeCheck(Scope& scope) {
    if (type == Type::ID) {
        // Es una variable: busca en la tabla
        const std::string& name = std::get<std::string>(value);
        TableValue* val = scope.lookup(name);
        if (!val)
            throw std::runtime_error(std::format("'{}' no fue declarado", name));
        if (!std::holds_alternative<VarInfo>(*val))
            throw std::runtime_error(std::format("'{}' es una función, no una variable", name));
        this->type = std::get<VarInfo>(*val).type;
    }
    // INT, BOOL, STRING → el tipo ya fue asignado en el constructor
}

void unaryexpr::typeCheck(Scope& scope) {
    expr->typeCheck(scope);

    if (op == UNARY::EMPTY) {
        this->type = expr->type;
        return;
    }

    if (op == UNARY::MINUS) {
        if (expr->type != Type::INT)
            throw std::runtime_error(std::format(
                "'-' unario requiere int, no '{}'", typeName(expr->type)));
        this->type = Type::INT;
    }

    if (op == UNARY::NOT) {
        if (expr->type != Type::BOOL)
            throw std::runtime_error(std::format(
                "'!' requiere bool, no '{}'", typeName(expr->type)));
        this->type = Type::BOOL;
    }
}

void binaryexpr::typeCheck(Scope& scope) {
    left->typeCheck(scope);

    // EMPTY significa que solo hay un lado (wrap sin operador real)
    if (op == RELATIONAL::EMPTY) {
        this->type = left->type;
        return;
    }

    right->typeCheck(scope);

    if (isArithOp(op)) {
        // int OP int → int
        if (left->type != Type::INT)
            throw std::runtime_error(std::format(
                "operador aritmético: operando izquierdo debe ser int, no '{}'", typeName(left->type)));
        if (right->type != Type::INT)
            throw std::runtime_error(std::format(
                "operador aritmético: operando derecho debe ser int, no '{}'", typeName(right->type)));
        this->type = Type::INT;
    }
    else if (isRelOp(op)) {
        // int OP int → bool
        if (left->type != Type::INT)
            throw std::runtime_error(std::format(
                "operador relacional: operando izquierdo debe ser int, no '{}'", typeName(left->type)));
        if (right->type != Type::INT)
            throw std::runtime_error(std::format(
                "operador relacional: operando derecho debe ser int, no '{}'", typeName(right->type)));
        this->type = Type::BOOL;
    }
    else if (isEqOp(op)) {
        // T == T → bool  (T puede ser int o bool, pero ambos deben ser iguales)
        if (left->type != right->type)
            throw std::runtime_error(std::format(
                "'==' / '!=': los dos lados deben ser del mismo tipo ('{}' vs '{}')",
                typeName(left->type), typeName(right->type)));
        this->type = Type::BOOL;
    }
    else if (isLogicOp(op)) {
        // bool OP bool → bool
        if (left->type != Type::BOOL)
            throw std::runtime_error(std::format(
                "operador lógico: operando izquierdo debe ser bool, no '{}'", typeName(left->type)));
        if (right->type != Type::BOOL)
            throw std::runtime_error(std::format(
                "operador lógico: operando derecho debe ser bool, no '{}'", typeName(right->type)));
        this->type = Type::BOOL;
    }
}