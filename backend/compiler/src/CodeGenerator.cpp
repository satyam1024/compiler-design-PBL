#include "CodeGenerator.h"
#include <sstream>
#include <cctype>

CodeGenerator::CodeGenerator() {}

void CodeGenerator::generate(const std::vector<IRInstruction>& ir) {
    cCode.clear();
    declaredVars.clear();

    std::ostringstream oss;
    oss << "#include <stdio.h>\n\nint main() {\n";

    // First pass: declare all variables used as targets
    for (const auto& instr : ir) {
        if (instr.opcode == "ASSIGN" && instr.operands.size() == 2) {
            declareVar(instr.operands[1]);
        } else if ((instr.opcode == "ADD" || instr.opcode == "SUB" ||
                    instr.opcode == "MUL" || instr.opcode == "DIV" ||
                    instr.opcode == "LE" || instr.opcode == "LT" ||
                    instr.opcode == "GT" || instr.opcode == "GE" ||
                    instr.opcode == "EQ" || instr.opcode == "NE") &&
                   instr.operands.size() == 3) {
            declareVar(instr.operands[2]);
        } else if (instr.opcode == "INPUT" && instr.operands.size() == 1) {
            declareVar(instr.operands[0]);
        }
    }

    // Declare all variables as double for simplicity
    for (const auto& var : declaredVars) {
        oss << "    double " << var << " = 0;\n";
    }

    // For string literals, use const char* in output
    bool needsStringOutput = false;

    // Second pass: emit code
    std::vector<std::string> labelStack;
    for (const auto& instr : ir) {
        if (instr.opcode == "ASSIGN" && instr.operands.size() == 2) {
            oss << "    " << instr.operands[1] << " = " << instr.operands[0] << ";\n";
        } else if (instr.opcode == "ADD" && instr.operands.size() == 3) {
            oss << "    " << instr.operands[2] << " = " << instr.operands[0] << " + " << instr.operands[1] << ";\n";
        } else if (instr.opcode == "SUB" && instr.operands.size() == 3) {
            oss << "    " << instr.operands[2] << " = " << instr.operands[0] << " - " << instr.operands[1] << ";\n";
        } else if (instr.opcode == "MUL" && instr.operands.size() == 3) {
            oss << "    " << instr.operands[2] << " = " << instr.operands[0] << " * " << instr.operands[1] << ";\n";
        } else if (instr.opcode == "DIV" && instr.operands.size() == 3) {
            oss << "    " << instr.operands[2] << " = " << instr.operands[0] << " / " << instr.operands[1] << ";\n";
        } else if ((instr.opcode == "LE" || instr.opcode == "LT" ||
                    instr.opcode == "GT" || instr.opcode == "GE" ||
                    instr.opcode == "EQ" || instr.opcode == "NE") &&
                   instr.operands.size() == 3) {
            std::string op;
            if (instr.opcode == "LE") op = "<=";
            else if (instr.opcode == "LT") op = "<";
            else if (instr.opcode == "GT") op = ">";
            else if (instr.opcode == "GE") op = ">=";
            else if (instr.opcode == "EQ") op = "==";
            else if (instr.opcode == "NE") op = "!=";
            oss << "    " << instr.operands[2] << " = (" << instr.operands[0] << " " << op << " " << instr.operands[1] << ");\n";
        } else if (instr.opcode == "INPUT" && instr.operands.size() == 1) {
            oss << "    printf(\"Enter value for " << instr.operands[0] << ": \");\n";
            oss << "    scanf(\"%lf\", &" << instr.operands[0] << ");\n";
        } else if (instr.opcode == "OUTPUT" && instr.operands.size() == 1) {
            // Check if it's a string literal
            if (!instr.operands[0].empty() && instr.operands[0][0] == '"') {
                needsStringOutput = true;
                oss << "    printf(" << instr.operands[0] << ");\n";
            } else {
                oss << "    printf(\"%lf\\n\", " << instr.operands[0] << ");\n";
            }
        } else if (instr.opcode == "LABEL" && instr.operands.size() == 1) {
            oss << instr.operands[0] << ":\n";
        } else if (instr.opcode == "JMP" && instr.operands.size() == 1) {
            oss << "    goto " << instr.operands[0] << ";\n";
        } else if (instr.opcode == "JZ" && instr.operands.size() == 2) {
            oss << "    if (!" << instr.operands[0] << ") goto " << instr.operands[1] << ";\n";
        } else if (instr.opcode == "JNZ" && instr.operands.size() == 2) {
            oss << "    if (" << instr.operands[0] << ") goto " << instr.operands[1] << ";\n";
        }
    }

    oss << "    return 0;\n}\n";
    cCode = oss.str();
}

const std::string& CodeGenerator::getCCode() const {
    return cCode;
}

void CodeGenerator::declareVar(const std::string& var) {
    // Only declare if not a literal or already declared
    if (var.empty()) return;
    if (var[0] == '"' || isNumber(var)) return;
    declaredVars.insert(var);
}

bool CodeGenerator::isNumber(const std::string& s) const {
    if (s.empty()) return false;
    char* end = nullptr;
    std::strtod(s.c_str(), &end);
    return end && *end == '\0';
}
