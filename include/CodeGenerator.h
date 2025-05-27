#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "Optimizer.h"
#include <string>
#include <vector>
#include <unordered_set>

class CodeGenerator {
public:
    CodeGenerator();
    void generate(const std::vector<IRInstruction>& ir);
    const std::string& getCCode() const;

private:
    std::string cCode;
    std::unordered_set<std::string> declaredVars;

    void declareVar(const std::string& var);
    bool isNumber(const std::string& s) const;
};

#endif // CODE_GENERATOR_H
