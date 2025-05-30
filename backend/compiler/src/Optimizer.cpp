#include "Optimizer.h"
#include <sstream>
#include <unordered_map>

Optimizer::Optimizer() {}

void Optimizer::optimize(const std::vector<IRInstruction>& inputIR) {
    optimizedIR = inputIR;
    constantFolding();
    removeRedundantAssignments();
}

const std::vector<IRInstruction>& Optimizer::getOptimizedIR() const {
    return optimizedIR;
}

// Helper to check if a string is a number (integer or float)
bool Optimizer::isNumber(const std::string& s) const {
    if (s.empty()) return false;
    std::istringstream iss(s);
    double d;
    iss >> d;
    return iss.eof() && !iss.fail();
}

// Constant folding for ADD, SUB, MUL, DIV
void Optimizer::constantFolding() {
    for (auto& instr : optimizedIR) {
        if ((instr.opcode == "ADD" || instr.opcode == "SUB" ||
             instr.opcode == "MUL" || instr.opcode == "DIV") &&
            instr.operands.size() == 3 &&
            isNumber(instr.operands[0]) &&
            isNumber(instr.operands[1])) {
            double left = std::stod(instr.operands[0]);
            double right = std::stod(instr.operands[1]);
            double result = 0.0;
            if (instr.opcode == "ADD") result = left + right;
            else if (instr.opcode == "SUB") result = left - right;
            else if (instr.opcode == "MUL") result = left * right;
            else if (instr.opcode == "DIV") result = (right != 0.0) ? left / right : 0.0;
            instr.opcode = "ASSIGN";
            instr.operands = { std::to_string(result), instr.operands[2] };
        }
    }
}

// Remove redundant assignments like ASSIGN x, x;
void Optimizer::removeRedundantAssignments() {
    std::vector<IRInstruction> filtered;
    for (const auto& instr : optimizedIR) {
        if (instr.opcode == "ASSIGN" &&
            instr.operands.size() == 2 &&
            instr.operands[0] == instr.operands[1]) {
            // Skip redundant assignment
            continue;
        }
        filtered.push_back(instr);
    }
    optimizedIR = std::move(filtered);
}
