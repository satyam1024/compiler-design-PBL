#include "CodeGenerator.h"
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <map>

CodeGenerator::CodeGenerator() {}

std::string promote(const std::string& t1, const std::string& t2) {
    if (t1 == "const char*" || t2 == "const char*") return "const char*";
    if (t1 == "double" || t2 == "double") return "double";
    return "int";
}

bool CodeGenerator::isIdentifier(const std::string& s) const {
    if (s.empty()) return false;
    if (!(isalpha(s[0]) || s[0] == '_')) return false;
    for (char c : s) {
        if (!(isalnum(c) || c == '_')) return false;
    }
    return true;
}

void CodeGenerator::generate(const std::vector<IRInstruction>& ir) {
    cCode.clear();
    declaredVars.clear();

    std::ostringstream oss;
    oss << "#include <stdio.h>\n\nint main() {\n";

    for (const auto& instr : ir) {
        if (instr.opcode == "ASSIGN" && instr.operands.size() == 2) {
            declareVar(instr.operands[1], instr.operands[0]);
        } else if ((instr.opcode == "ADD" || instr.opcode == "SUB" ||
                    instr.opcode == "MUL" || instr.opcode == "DIV") &&
                   instr.operands.size() == 3) {
            std::string t1 = declaredVars[instr.operands[0]];
            std::string t2 = declaredVars[instr.operands[1]];
            std::string resultType = promote(t1, t2);
            declaredVars[instr.operands[2]] = resultType;
        } else if ((instr.opcode == "LE" || instr.opcode == "LT" ||
                    instr.opcode == "GT" || instr.opcode == "GE" ||
                    instr.opcode == "EQ" || instr.opcode == "NE") &&
                   instr.operands.size() == 3) {
            declaredVars[instr.operands[2]] = "int"; 
        } else if (instr.opcode == "INPUT" && instr.operands.size() == 1) {
            declareVar(instr.operands[0], "");
        }
    }

    for (const auto& entry : declaredVars) {
        const std::string& var = entry.first;
        const std::string& type = entry.second;
        if (isIdentifier(var))
            oss << "    " << type << " " << var << " = 0;\n";
    }

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
            const std::string& var = instr.operands[0];

            if (!var.empty() && var[0] == '"') {
                oss << "    printf(" << var << ");\n";
            } else if (declaredVars.count(var)) {
                const std::string& type = declaredVars[var];
                if (type == "int")
                    oss << "    printf(\"%d\\n\", " << var << ");\n";
                else if (type == "double")
                    oss << "    printf(\"%lf\\n\", " << var << ");\n";
                else if (type == "const char*")
                    oss << "    printf(\"%s\\n\", " << var << ");\n";
            } else {
                oss << "    printf(\"%lf\\n\", " << var << ");\n";
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

void CodeGenerator::declareVar(const std::string& var, const std::string& value) {
    if (var.empty() || declaredVars.count(var)) return;

    if (value.empty()) {
        declaredVars[var] = "double"; 
        return;
    }

    if (value[0] == '"') {
        declaredVars[var] = "const char*";
    } else if (isInteger(value)) {
        declaredVars[var] = "int";
    } else if (isDouble(value)) {
        declaredVars[var] = "double";
    } else if (declaredVars.count(value)) {
        declaredVars[var] = declaredVars[value];
    } else {
        declaredVars[var] = "double";
    }
}

bool CodeGenerator::isInteger(const std::string& s) const {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool CodeGenerator::isDouble(const std::string& s) const {
    if (s.empty()) return false;
    bool dot = false;
    for (char c : s) {
        if (c == '.') {
            if (dot) return false;
            dot = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    return dot;
}

const std::string& CodeGenerator::getCCode() const {
    return cCode;
}
