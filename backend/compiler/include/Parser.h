#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include <memory>
#include <vector>
#include <string>

// AST Node base
struct ASTNode {
    virtual ~ASTNode() = default;
    int line;
    int column;
};

// Statement nodes
struct Statement : public ASTNode {};
struct Expression : public ASTNode {};

// Variable declaration: let a be 10
struct VarDecl : public Statement {
    std::string varName;
    std::unique_ptr<Expression> value;
};

// Assignment: let a be 10 (same as VarDecl in your language)
using Assignment = VarDecl;

// Input: input a
struct InputStmt : public Statement {
    std::string varName;
};

// Output: output a or output "Hello"
struct OutputStmt : public Statement {
    std::unique_ptr<Expression> value;
};

// Binary Operation: add a and b store in c, etc.
enum class BinOpType { ADD, SUBTRACT, MULTIPLY, DIVIDE };
struct BinOpStmt : public Statement {
    BinOpType op;
    std::string left;
    std::string right;
    std::string result;
};

// If-Else
struct IfStmt : public Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseIfBranches;
    std::vector<std::unique_ptr<Statement>> elseBranch;
};

// Repeat loop
struct RepeatStmt : public Statement {
    // For: repeat from i=0 to 10 jump 1
    std::string varName;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    std::unique_ptr<Expression> jump;
    std::vector<std::unique_ptr<Statement>> body;

    // Until: repeat until a<b
    std::unique_ptr<Expression> untilCondition;
};

// Expressions
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
    std::string left;
    std::string op;
    std::string right;
};

// Program root
struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;
};

// Parser class
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

#endif // PARSER_H
