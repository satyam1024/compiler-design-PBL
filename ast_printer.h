#pragma once
#include "ast.h"
#include <iostream>
#include <string>

void printAST(const std::shared_ptr<ASTNode>& node, int indent = 0) {
    std::string pad(indent, ' ');
    std::cout << pad << "Node Type: ";
    switch (node->type) {
        case ASTNodeType::Declaration: std::cout << "Declaration"; break;
        case ASTNodeType::Output: std::cout << "Output"; break;
        case ASTNodeType::Literal: std::cout << "Literal"; break;
        case ASTNodeType::Identifier: std::cout << "Identifier"; break;
    }
    std::cout << ", Value: " << node->value << "\n";
    for (auto& child : node->children) {
        printAST(child, indent + 2);
    }
}
