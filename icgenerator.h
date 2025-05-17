#ifndef ICGENERATOR_H
#define ICGENERATOR_H

#include "tokens.h"
#include <vector>
#include <string>

class ICGenerator {
public:
    ICGenerator(const std::vector<Token>& tokens);
    std::vector<std::string> generate();
    std::vector<std::string> getIntermediateCode(); // ✅ NEW

private:
    std::vector<Token> tokens;
    size_t pos;
    int tempCount;
    std::vector<std::string> intermediateCode; // ✅ NEW

    std::string newTemp();
    Token peek();
    Token advance();
    void handleDeclaration();
    void handleOutput();
};

#endif
