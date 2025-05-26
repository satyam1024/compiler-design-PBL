// codegen.h
#pragma once
#include "ast.h"
#include <string>

class CodeGenerator {
public:
    std::string generate(ASTNodePtr node);
private:
    std::string gen(ASTNodePtr node, int indent = 0);
    std::string indent(int n);
};
