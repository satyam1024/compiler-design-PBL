#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include <vector>
#include <unordered_map>

struct Symbol {
    std::string type;
    bool isConstant;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    void parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    std::unordered_map<std::string, Symbol> symbolTable;

    Token current();
    Token advance();
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& msg);

    void statement();
    void declaration();
    void outputStatement();
};

#endif
