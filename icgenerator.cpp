#include "icgenerator.h"
#include <iostream>
#include<vector>
#include<string.h>
using namespace std;

ICGenerator::ICGenerator(const std::vector<Token>& tokens) : tokens(tokens), pos(0), tempCount(0) {}

std::string ICGenerator::newTemp() {
    return "t" + std::to_string(tempCount++);
}

Token ICGenerator::peek() {
    return pos < tokens.size() ? tokens[pos] : Token(T_EOF, "");
}

Token ICGenerator::advance() {
    return pos < tokens.size() ? tokens[pos++] : Token(T_EOF, "");
}

void ICGenerator::handleDeclaration() {
    advance(); // Skip type
    Token var = advance();
    Token assign = advance();

    if (assign.type == T_ASSIGN) {
        Token value = advance();
        std::string temp = newTemp();
        std::string line1 = temp + " = " + value.value;
        std::string line2 = var.value + " = " + temp;
        std::cout << line1 << "\n" << line2 << "\n";
        intermediateCode.push_back(line1);
        intermediateCode.push_back(line2);
    }

    if (peek().type == T_SEMI) advance(); // skip ';'
}

void ICGenerator::handleOutput() {
    advance(); // skip 'output'
    Token a = advance();
    std::string temp = newTemp();
    std::string line1 = temp + " = " + a.value;
    std::string line2 = "print " + temp;
    std::cout << line1 << "\n" << line2 << "\n";
    intermediateCode.push_back(line1);
    intermediateCode.push_back(line2);

    if (peek().type == T_SEMI) advance(); // skip ';'
}

vector<string> ICGenerator::generate() {
    intermediateCode.clear(); // Clear old code
    pos = 0;                  // Reset position

    while (pos < tokens.size()) {
        Token tok = peek();
        if (tok.type == T_INT || tok.type == T_FLOAT || tok.type == T_STRING || tok.type == T_CHAR)
            handleDeclaration();
        else if (tok.type == T_OUTPUT)
            handleOutput();
        else
            advance();
    }

    return intermediateCode;
}


std::vector<std::string> ICGenerator::getIntermediateCode() {
    return intermediateCode;
}
