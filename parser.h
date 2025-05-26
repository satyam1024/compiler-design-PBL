// parser.h
#pragma once
#include "lexer.h"
#include "ast.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    ASTNodePtr parseProgram();
private:
    std::vector<Token> tokens;
    size_t pos;
    Token peek();
    Token get();
    bool match(TokenType type);
    ASTNodePtr parseStatement();
    ASTNodePtr parseVarDecl();
    ASTNodePtr parsePrint();
    ASTNodePtr parseInput();
    ASTNodePtr parseIf();
    ASTNodePtr parseFor();
    ASTNodePtr parseLoop();
    ASTNodePtr parseExpression();
};
