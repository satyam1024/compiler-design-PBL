#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include <string>

class CodeGenerator {
public:
    static void generate(const std::vector<std::string>& optimizedIC);
};

#endif
