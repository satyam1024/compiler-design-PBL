// ast.h
#pragma once
#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    PROGRAM, VAR_DECL, IF, FOR, LOOP, INPUT, PRINT, BLOCK, ASSIGN, LITERAL, IDENT
};

struct ASTNode {
    ASTNodeType type;
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> statements;
    ProgramNode() { type = ASTNodeType::PROGRAM; }
};

struct VarDeclNode : ASTNode {
    std::string name, varType;
    ASTNodePtr value;
    VarDeclNode(const std::string& n, const std::string& t, ASTNodePtr v)
        : name(n), varType(t), value(v) { type = ASTNodeType::VAR_DECL; }
};

struct LiteralNode : ASTNode {
    std::string value;
    LiteralNode(const std::string& val) : value(val) { type = ASTNodeType::LITERAL; }
};

struct IdentNode : ASTNode {
    std::string name;
    IdentNode(const std::string& n) : name(n) { type = ASTNodeType::IDENT; }
};

struct PrintNode : ASTNode {
    ASTNodePtr expr;
    PrintNode(ASTNodePtr e) : expr(e) { type = ASTNodeType::PRINT; }
};

struct InputNode : ASTNode {
    std::string varName;
    InputNode(const std::string& n) : varName(n) { type = ASTNodeType::INPUT; }
};

struct BlockNode : ASTNode {
    std::vector<ASTNodePtr> statements;
    BlockNode() { type = ASTNodeType::BLOCK; }
};

struct IfNode : ASTNode {
    ASTNodePtr condition;
    ASTNodePtr thenBlock;
    ASTNodePtr elseBlock;
    IfNode(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB)
        : condition(cond), thenBlock(thenB), elseBlock(elseB) { type = ASTNodeType::IF; }
};

struct ForNode : ASTNode {
    std::string var;
    ASTNodePtr start, end, block;
    ForNode(const std::string& v, ASTNodePtr s, ASTNodePtr e, ASTNodePtr b)
        : var(v), start(s), end(e), block(b) { type = ASTNodeType::FOR; }
};

struct LoopNode : ASTNode {
    ASTNodePtr times, block;
    LoopNode(ASTNodePtr t, ASTNodePtr b) : times(t), block(b) { type = ASTNodeType::LOOP; }
};
