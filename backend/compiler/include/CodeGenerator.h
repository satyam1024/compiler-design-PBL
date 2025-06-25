#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "IntermediateCodeGen.h"
#include <string>
#include <vector>
#include <unordered_map>

class CodeGenerator {
public:
    CodeGenerator();
    void generate(const std::vector<IRInstruction>& ir);
    const std::string& getCCode() const;

private:
    std::string cCode;
    bool isIdentifier(const std::string& s) const;

    std::unordered_map<std::string, std::string> declaredVars;

    void declareVar(const std::string& var, const std::string& value);

    bool isInteger(const std::string& s) const;
    bool isDouble(const std::string& s) const;
  
    void emitSingleStatement(const IRInstruction& instr, std::ostringstream& oss);
};

#endif 
