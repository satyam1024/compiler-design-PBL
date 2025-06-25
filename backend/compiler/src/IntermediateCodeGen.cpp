#include "IntermediateCodeGen.h"
#include <sstream>

IntermediateCodeGen::IntermediateCodeGen() : tempVarCounter(0) {}

void IntermediateCodeGen::generate(const Program* program) {
    ir.clear();
    tempVarCounter = 0;
    for (const auto& stmt : program->statements) {
        genStatement(stmt.get());
    }
}

const std::vector<IRInstruction>& IntermediateCodeGen::getIR() const {
    return ir;
}

std::string IntermediateCodeGen::newTemp() {
    return "_t" + std::to_string(tempVarCounter++);
}

std::string IntermediateCodeGen::relOpToOpcode(const std::string& op) {
    if (op == "==") return "EQ";
    if (op == "!=") return "NE";
    if (op == "<")  return "LT";
    if (op == "<=") return "LE";
    if (op == ">")  return "GT";
    if (op == ">=") return "GE";
    return "INVALID";
}

void IntermediateCodeGen::genStatement(const Statement* stmt) {
    if (!stmt) return;

    if (auto varDecl = dynamic_cast<const VarDecl*>(stmt)) {
        std::string rhs;
        genExpression(varDecl->value.get(), rhs);
        ir.emplace_back("ASSIGN", std::vector<std::string>{rhs, varDecl->varName}, stmt->line);
        return;
    }

    if (auto inputStmt = dynamic_cast<const InputStmt*>(stmt)) {
        ir.emplace_back("INPUT", std::vector<std::string>{inputStmt->varName}, stmt->line);
        return;
    }

    if (auto outputStmt = dynamic_cast<const OutputStmt*>(stmt)) {
        std::string val;
        genExpression(outputStmt->value.get(), val);
        ir.emplace_back("OUTPUT", std::vector<std::string>{val}, stmt->line);
        return;
    }

    if (auto binOp = dynamic_cast<const BinOpStmt*>(stmt)) {
        std::string opStr;
        switch (binOp->op) {
            case BinOpType::ADD: opStr = "ADD"; break;
            case BinOpType::SUBTRACT: opStr = "SUB"; break;
            case BinOpType::MULTIPLY: opStr = "MUL"; break;
            case BinOpType::DIVIDE: opStr = "DIV"; break;
        }
        ir.emplace_back(opStr, std::vector<std::string>{binOp->left, binOp->right, binOp->result}, stmt->line);
        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        std::string cond;
        genExpression(ifStmt->condition.get(), cond);
        std::string labelElse = "L" + std::to_string(tempVarCounter++);
        std::string labelEnd = "L" + std::to_string(tempVarCounter++);
        ir.emplace_back("JZ", std::vector<std::string>{cond, labelElse}, stmt->line);

        for (const auto& s : ifStmt->thenBranch) genStatement(s.get());
        ir.emplace_back("JMP", std::vector<std::string>{labelEnd}, stmt->line);

        ir.emplace_back("LABEL", std::vector<std::string>{labelElse}, stmt->line);
        for (const auto& s : ifStmt->elseBranch) genStatement(s.get());  

        ir.emplace_back("LABEL", std::vector<std::string>{labelEnd}, stmt->line);
        return;
    }

    if (auto repeatStmt = dynamic_cast<const RepeatStmt*>(stmt)) {
        if (!repeatStmt->varName.empty()) {
            std::string startVal, endVal, jumpVal;
            genExpression(repeatStmt->start.get(), startVal);
            genExpression(repeatStmt->end.get(), endVal);
            genExpression(repeatStmt->jump.get(), jumpVal);

            ir.emplace_back("ASSIGN", std::vector<std::string>{startVal, repeatStmt->varName}, stmt->line);

            std::string labelStart = "L" + std::to_string(tempVarCounter++);
            std::string labelEnd = "L" + std::to_string(tempVarCounter++);
            ir.emplace_back("LABEL", std::vector<std::string>{labelStart}, stmt->line);

            std::string condTemp = newTemp();
            ir.emplace_back("LE", std::vector<std::string>{repeatStmt->varName, endVal, condTemp}, stmt->line);
            ir.emplace_back("JZ", std::vector<std::string>{condTemp, labelEnd}, stmt->line);

            for (const auto& s : repeatStmt->body) genStatement(s.get());

            std::string incTemp = newTemp();
            ir.emplace_back("ADD", std::vector<std::string>{repeatStmt->varName, jumpVal, incTemp}, stmt->line);
            ir.emplace_back("ASSIGN", std::vector<std::string>{incTemp, repeatStmt->varName}, stmt->line);

            ir.emplace_back("JMP", std::vector<std::string>{labelStart}, stmt->line);
            ir.emplace_back("LABEL", std::vector<std::string>{labelEnd}, stmt->line);
        }
        else if (repeatStmt->untilCondition) {
            std::string labelStart = "L" + std::to_string(tempVarCounter++);
            std::string labelEnd = "L" + std::to_string(tempVarCounter++);
            ir.emplace_back("LABEL", std::vector<std::string>{labelStart}, stmt->line);

            std::string cond;
            genExpression(repeatStmt->untilCondition.get(), cond);
            ir.emplace_back("JNZ", std::vector<std::string>{cond, labelEnd}, stmt->line);

            for (const auto& s : repeatStmt->body) genStatement(s.get());

            ir.emplace_back("JMP", std::vector<std::string>{labelStart}, stmt->line);
            ir.emplace_back("LABEL", std::vector<std::string>{labelEnd}, stmt->line);
        }
        return;
    }
}

void IntermediateCodeGen::genExpression(const Expression* expr, std::string& result) {
    if (!expr) {
        result = "";
        return;
    }

    if (auto id = dynamic_cast<const Identifier*>(expr)) {
        result = id->name;
        return;
    }
    if (auto num = dynamic_cast<const NumberLiteral*>(expr)) {
        result = num->value;
        return;
    }
    if (auto str = dynamic_cast<const StringLiteral*>(expr)) {
        result = "\"" + str->value + "\"";
        return;
    }
    if (auto rel = dynamic_cast<const RelOpExpr*>(expr)) {
        std::string leftVal, rightVal;
        genExpression(rel->left.get(), leftVal);
        genExpression(rel->right.get(), rightVal);
        std::string temp = newTemp();
        std::string opcode = relOpToOpcode(rel->op);
        ir.emplace_back(opcode, std::vector<std::string>{leftVal, rightVal, temp}, expr->line);
        result = temp;
        return;
    }

    result = "";
}
