// ast.h
#pragma once
#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    Declaration,
    Output,
    Literal,
    Identifier
};

struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::string dataType;
    bool isConst = false;

    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType t, const std::string& val = "") : type(t), value(val) {}
};
