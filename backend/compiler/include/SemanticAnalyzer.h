#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "Parser.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

enum class VarType { NUMBER, STRING, BOOLEAN, UNKNOWN };


struct VariableInfo {
    VarType type;
    int lineDeclared;
};

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    void analyze(const Program* program);
    const std::vector<std::string>& getErrors() const;

private:
    std::unordered_map<std::string, VariableInfo> symbolTable;
    std::vector<std::string> errors;

    void analyzeStatement(const Statement* stmt);
    void analyzeExpression(const Expression* expr, VarType& outType);

    void declareVariable(const std::string& name, VarType type, int line);
    bool isVariableDeclared(const std::string& name) const;
    VarType getVariableType(const std::string& name) const;
};
std::string varTypeToString(VarType type);

#endif
