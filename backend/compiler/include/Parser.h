#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <memory>
#include <vector>
#include <string>

struct ASTNode {
    virtual ~ASTNode() = default;
    int line;
    int column;
};

struct Statement : public ASTNode {};
struct Expression : public ASTNode {};

struct VarDecl : public Statement {
    std::string varName;
    std::unique_ptr<Expression> value;
};

using Assignment = VarDecl;

struct InputStmt : public Statement {
    std::string varName;
};

struct OutputStmt : public Statement {
    std::unique_ptr<Expression> value;
};

enum class BinOpType { ADD, SUBTRACT, MULTIPLY, DIVIDE };
struct BinOpStmt : public Statement {
    BinOpType op;
    std::string left;
    std::string right;
    std::string result;
};

struct IfStmt : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseIfBranches;
    std::vector<std::unique_ptr<Statement>> elseBranch;
};

struct RepeatStmt : public Statement {
    std::string varName;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    std::unique_ptr<Expression> jump;
    std::vector<std::unique_ptr<Statement>> body;

    std::unique_ptr<Expression> untilCondition;
};

struct Identifier : public Expression {
    std::string name;
};

struct NumberLiteral : public Expression {
    std::string value;
};

struct StringLiteral : public Expression {
    std::string value;
};

struct RelOpExpr : public Expression {
    std::unique_ptr<Expression> left;
    std::string op; 
    std::unique_ptr<Expression> right;
};

struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
    std::vector<std::string> getErrors() const;

private:
    const std::vector<Token>& tokens;
    size_t pos;
    std::vector<std::string> errors;

    const Token& peek() const;
    const Token& get();
    bool match(TokenType type);
    bool matchKeyword(TokenType type);
    void expect(TokenType type, const std::string& errorMsg);

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseVarDecl();
    std::unique_ptr<Statement> parseInput();
    std::unique_ptr<Statement> parseOutput();
    std::unique_ptr<Statement> parseBinOp();
    std::unique_ptr<Statement> parseIf();
    std::unique_ptr<Statement> parseRepeat();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseRelOpExpr();
    std::unique_ptr<Expression> parsePrimary();
};

#endif 
