// codegen.cpp
#include <algorithm> 
#include "codegen.h"
#include <sstream>

std::string CodeGenerator::indent(int n) { return std::string(n * 2, ' '); }

std::string CodeGenerator::generate(ASTNodePtr node) {
    std::ostringstream oss;
    oss << "#include <stdio.h>\nint main() {\n";
    oss << gen(node, 1);
    oss << "  return 0;\n}\n";
    return oss.str();
}

std::string CodeGenerator::gen(ASTNodePtr node, int ind) {
    std::ostringstream oss;
    if (!node) return "";
    switch (node->type) {
    case ASTNodeType::PROGRAM: {
        auto prog = std::dynamic_pointer_cast<ProgramNode>(node);
        for (auto& stmt : prog->statements)
            oss << gen(stmt, ind);
        break;
    }
    case ASTNodeType::VAR_DECL: {
        auto var = std::dynamic_pointer_cast<VarDeclNode>(node);
        oss << indent(ind) << var->varType << " " << var->name << " = ";
        oss << gen(var->value, 0) << ";\n";
        break;
    }
    case ASTNodeType::LITERAL: {
        auto lit = std::dynamic_pointer_cast<LiteralNode>(node);
        oss << lit->value;
        break;
    }
    case ASTNodeType::IDENT: {
        auto id = std::dynamic_pointer_cast<IdentNode>(node);
        oss << id->name;
        break;
    }
    case ASTNodeType::PRINT: {
        auto pr = std::dynamic_pointer_cast<PrintNode>(node);
        oss << indent(ind) << "printf(\"";
        // Only handle int and string for demo
        if (pr->expr->type == ASTNodeType::LITERAL) {
            auto lit = std::dynamic_pointer_cast<LiteralNode>(pr->expr);
            bool isNum = std::all_of(lit->value.begin(), lit->value.end(), ::isdigit);
            oss << (isNum ? "%d" : "%s") << "\", ";
            if (isNum) oss << lit->value;
            else oss << "\"" << lit->value << "\"";
        } else if (pr->expr->type == ASTNodeType::IDENT) {
            oss << "%d\", " << gen(pr->expr, 0);
        }
        oss << ");\n";
        break;
    }
    case ASTNodeType::INPUT: {
        auto in = std::dynamic_pointer_cast<InputNode>(node);
        oss << indent(ind) << "scanf(\"%d\", &" << in->varName << ");\n";
        break;
    }
    case ASTNodeType::IF: {
        auto ifn = std::dynamic_pointer_cast<IfNode>(node);
        oss << indent(ind) << "if (" << gen(ifn->condition, 0) << ") {\n";
        oss << gen(ifn->thenBlock, ind + 1);
        oss << indent(ind) << "}";
        if (ifn->elseBlock) {
            oss << " else {\n" << gen(ifn->elseBlock, ind + 1) << indent(ind) << "}";
        }
        oss << "\n";
        break;
    }
    case ASTNodeType::FOR: {
        auto forn = std::dynamic_pointer_cast<ForNode>(node);
        oss << indent(ind) << "for (int " << forn->var << " = " << gen(forn->start, 0)
            << "; " << forn->var << " <= " << gen(forn->end, 0) << "; ++" << forn->var << ") {\n";
        oss << gen(forn->block, ind + 1);
        oss << indent(ind) << "}\n";
        break;
    }
    case ASTNodeType::LOOP: {
        auto loopn = std::dynamic_pointer_cast<LoopNode>(node);
        oss << indent(ind) << "for (int __i = 0; __i < " << gen(loopn->times, 0) << "; ++__i) {\n";
        oss << gen(loopn->block, ind + 1);
        oss << indent(ind) << "}\n";
        break;
    }
    case ASTNodeType::BLOCK: {
        auto block = std::dynamic_pointer_cast<BlockNode>(node);
        for (auto& stmt : block->statements)
            oss << gen(stmt, ind);
        break;
    }
    default: break;
    }
    return oss.str();
}
