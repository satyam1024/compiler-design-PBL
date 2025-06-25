#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "IntermediateCodeGen.h"
#include <vector>
#include <string>

class Optimizer {
public:
    Optimizer();
    void optimize(const std::vector<IRInstruction>& inputIR);
    const std::vector<IRInstruction>& getOptimizedIR() const;

private:
    std::vector<IRInstruction> optimizedIR;

    void constantFolding();
    void removeRedundantAssignments();
    bool isNumber(const std::string& s) const;
};

#endif 
