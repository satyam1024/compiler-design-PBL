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

    // ✅ Change from set<string> to map<string, string> to store variable types
    std::unordered_map<std::string, std::string> declaredVars;

    // ✅ Modified declareVar to take assigned value for type inference
    void declareVar(const std::string& var, const std::string& value);

    // ✅ Helpers to detect number types
    bool isInteger(const std::string& s) const;
    bool isDouble(const std::string& s) const;
};

#endif // CODE_GENERATOR_H
