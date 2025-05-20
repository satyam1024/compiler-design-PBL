// parser.h
#pragma once
#include "tokens.h"
#include "ast.h"
#include <vector>
#include <unordered_map>
#include <memory>

class Parser {
    std::vector<Token> tokens;
    size_t pos;

public:
    std::unordered_map<std::string, std::pair<std::string, bool>> symbolTable;
    std::vector<std::shared_ptr<ASTNode>> ast; // 🌱 AST root nodes

    Parser(const std::vector<Token>& tokens);
    void parse();

private:
    Token current();
    Token advance();
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& msg);

    void statement();
    void declaration();
    void outputStatement();
};
