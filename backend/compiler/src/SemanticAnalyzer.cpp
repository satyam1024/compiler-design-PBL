#include "SemanticAnalyzer.h"
#include <sstream>

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(const Program* program) {
    symbolTable.clear();
    errors.clear();
    for (const auto& stmt : program->statements) {
        analyzeStatement(stmt.get());
    }
}

void SemanticAnalyzer::analyzeStatement(const Statement* stmt) {
    if (!stmt) return;

    if (auto varDecl = dynamic_cast<const VarDecl*>(stmt)) {
        VarType exprType = VarType::UNKNOWN;
        analyzeExpression(varDecl->value.get(), exprType);

        if (symbolTable.count(varDecl->varName)) {
            std::stringstream ss;
            ss << "Line " << stmt->line << ": Variable '" << varDecl->varName << "' redeclared (previously declared at line "
               << symbolTable[varDecl->varName].lineDeclared << ").";
            errors.push_back(ss.str());
        } else {
            declareVariable(varDecl->varName, exprType, stmt->line);
        }
        return;
    }

    if (auto inputStmt = dynamic_cast<const InputStmt*>(stmt)) {
        if (!isVariableDeclared(inputStmt->varName)) {
            declareVariable(inputStmt->varName, VarType::NUMBER, stmt->line);
        }
        return;
    }

    if (auto outputStmt = dynamic_cast<const OutputStmt*>(stmt)) {
        VarType exprType = VarType::UNKNOWN;
        analyzeExpression(outputStmt->value.get(), exprType);
        return;
    }

    if (auto binOp = dynamic_cast<const BinOpStmt*>(stmt)) {
        VarType leftType = VarType::UNKNOWN;
        VarType rightType = VarType::UNKNOWN;

        if (!isVariableDeclared(binOp->left)) {
            errors.push_back("Line " + std::to_string(stmt->line) + ": Variable '" + binOp->left + "' not declared.");
        } else {
            leftType = getVariableType(binOp->left);
        }

        if (!isVariableDeclared(binOp->right)) {
            errors.push_back("Line " + std::to_string(stmt->line) + ": Variable '" + binOp->right + "' not declared.");
        } else {
            rightType = getVariableType(binOp->right);
        }

        if (leftType != VarType::NUMBER || rightType != VarType::NUMBER) {
            std::stringstream ss;
            ss << "Line " << stmt->line << ": Cannot perform binary operation on types ";
            ss << varTypeToString(leftType) << " and " << varTypeToString(rightType) << ".";
            errors.push_back(ss.str());
        }

        if (!isVariableDeclared(binOp->result)) {
            declareVariable(binOp->result, VarType::NUMBER, stmt->line);
        }

        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        VarType condType = VarType::UNKNOWN;
        analyzeExpression(ifStmt->condition.get(), condType);
        for (const auto& s : ifStmt->thenBranch) analyzeStatement(s.get());
        for (const auto& s : ifStmt->elseIfBranches) analyzeStatement(s.get());
        for (const auto& s : ifStmt->elseBranch) analyzeStatement(s.get());
        return;
    }

    if (auto repeatStmt = dynamic_cast<const RepeatStmt*>(stmt)) {
        if (!repeatStmt->varName.empty() && !isVariableDeclared(repeatStmt->varName)) {
            declareVariable(repeatStmt->varName, VarType::NUMBER, stmt->line);
        }
        if (repeatStmt->start) {
            VarType t = VarType::UNKNOWN;
            analyzeExpression(repeatStmt->start.get(), t);
        }
        if (repeatStmt->end) {
            VarType t = VarType::UNKNOWN;
            analyzeExpression(repeatStmt->end.get(), t);
        }
        if (repeatStmt->jump) {
            VarType t = VarType::UNKNOWN;
            analyzeExpression(repeatStmt->jump.get(), t);
        }
        if (repeatStmt->untilCondition) {
            VarType t = VarType::UNKNOWN;
            analyzeExpression(repeatStmt->untilCondition.get(), t);
        }
        for (const auto& s : repeatStmt->body) analyzeStatement(s.get());
        return;
    }
}

void SemanticAnalyzer::analyzeExpression(const Expression* expr, VarType& outType) {
    if (!expr) {
        outType = VarType::UNKNOWN;
        return;
    }

    if (auto id = dynamic_cast<const Identifier*>(expr)) {
        if (!isVariableDeclared(id->name)) {
            errors.push_back("Line " + std::to_string(expr->line) + ": Variable '" + id->name + "' not declared.");
            outType = VarType::UNKNOWN;
        } else {
            outType = getVariableType(id->name);
        }
        return;
    }

    if (auto num = dynamic_cast<const NumberLiteral*>(expr)) {
        outType = VarType::NUMBER;
        return;
    }

    if (auto str = dynamic_cast<const StringLiteral*>(expr)) {
        outType = VarType::STRING;
        return;
    }

    if (auto rel = dynamic_cast<const RelOpExpr*>(expr)) {
        VarType leftType = VarType::UNKNOWN;
        VarType rightType = VarType::UNKNOWN;

        analyzeExpression(rel->left.get(), leftType);
        analyzeExpression(rel->right.get(), rightType);

        bool valid = false;
        if (rel->op == "==" || rel->op == "!=") {
            valid = (leftType == rightType) &&
                    (leftType == VarType::NUMBER || leftType == VarType::STRING);
        } else {
            // <, >, <=, >=
            valid = (leftType == VarType::NUMBER && rightType == VarType::NUMBER);
        }
        if (!valid) {
            std::stringstream ss;
            ss << "Line " << expr->line << ": Cannot compare " << varTypeToString(leftType)
               << " and " << varTypeToString(rightType) << " using relational operator '" << rel->op << "'.";
            errors.push_back(ss.str());
        }

        outType = VarType::BOOLEAN;
        return;
    }

    outType = VarType::UNKNOWN;
}

void SemanticAnalyzer::declareVariable(const std::string& name, VarType type, int line) {
    symbolTable[name] = {type, line};
}

bool SemanticAnalyzer::isVariableDeclared(const std::string& name) const {
    return symbolTable.count(name) > 0;
}

VarType SemanticAnalyzer::getVariableType(const std::string& name) const {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) return it->second.type;
    return VarType::UNKNOWN;
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

std::string varTypeToString(VarType type) {
    switch (type) {
        case VarType::NUMBER: return "NUMBER";
        case VarType::STRING: return "STRING";
        case VarType::BOOLEAN: return "BOOLEAN";
        case VarType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}
