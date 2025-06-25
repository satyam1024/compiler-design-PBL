#ifndef INTERMEDIATE_CODE_GEN_H
#define INTERMEDIATE_CODE_GEN_H

#include "Parser.h"
#include <vector>
#include <string>
#include <memory>

struct IRInstruction {
    std::string opcode;
    std::vector<std::string> operands;
    int line; 

    IRInstruction(const std::string& op, const std::vector<std::string>& ops, int ln)
        : opcode(op), operands(ops), line(ln) {}
};

class IntermediateCodeGen {
public:
    IntermediateCodeGen();
    void generate(const Program* program);
    const std::vector<IRInstruction>& getIR() const;

private:
    std::vector<IRInstruction> ir;

    std::string relOpToOpcode(const std::string& op);


    void genStatement(const Statement* stmt);
    void genExpression(const Expression* expr, std::string& result);

    int tempVarCounter;
    std::string newTemp();
};

#endif 
