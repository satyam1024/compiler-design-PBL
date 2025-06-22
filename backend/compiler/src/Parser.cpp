#include "Parser.h"
#include <iostream>

// Helper macros
#define CURRENT_TOKEN (pos < tokens.size() ? tokens[pos] : tokens.back())

Parser::Parser(const std::vector<Token>& tks)
    : tokens(tks), pos(0) {}

const Token& Parser::peek() const {
    return CURRENT_TOKEN;
}

const Token& Parser::get() {
    if (pos < tokens.size()) return tokens[pos++];
    return tokens.back();
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        get();
        return true;
    }
    return false;
}

bool Parser::matchKeyword(TokenType type) {
    return match(type);
}

void Parser::expect(TokenType type, const std::string& errorMsg) {
    if (!match(type)) {
        errors.push_back("Line " + std::to_string(peek().line) + ": " + errorMsg);
    }
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (peek().type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) program->statements.push_back(std::move(stmt));
        else get(); // Skip to next token on error
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (peek().type == TokenType::LET) return parseVarDecl();
    if (peek().type == TokenType::INPUT) return parseInput();
    if (peek().type == TokenType::OUTPUT) return parseOutput();
    if (peek().type == TokenType::ADD || peek().type == TokenType::SUBTRACT ||
        peek().type == TokenType::MULTIPLY || peek().type == TokenType::DIVIDE)
        return parseBinOp();
    if (peek().type == TokenType::IF) return parseIf();
    if (peek().type == TokenType::REPEAT) return parseRepeat();
    // Could add more statements here
    errors.push_back("Line " + std::to_string(peek().line) + ": Unexpected statement.");
    return nullptr;
}

std::unique_ptr<Statement> Parser::parseVarDecl() {
    auto stmt = std::make_unique<VarDecl>();
    stmt->line = peek().line;
    stmt->column = peek().column;
    expect(TokenType::LET, "Expected 'let'");
    if (peek().type != TokenType::IDENTIFIER) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected variable name.");
        return nullptr;
    }
    stmt->varName = get().lexeme;
    expect(TokenType::BE, "Expected 'be'");
    stmt->value = parseExpression();
    return stmt;
}

std::unique_ptr<Statement> Parser::parseInput() {
    auto stmt = std::make_unique<InputStmt>();
    stmt->line = peek().line;
    stmt->column = peek().column;
    expect(TokenType::INPUT, "Expected 'input'");
    if (peek().type != TokenType::IDENTIFIER) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected variable name after 'input'.");
        return nullptr;
    }
    stmt->varName = get().lexeme;
    return stmt;
}

std::unique_ptr<Statement> Parser::parseOutput() {
    auto stmt = std::make_unique<OutputStmt>();
    stmt->line = peek().line;
    stmt->column = peek().column;
    expect(TokenType::OUTPUT, "Expected 'output'");
    stmt->value = parseExpression();
    return stmt;
}

std::unique_ptr<Statement> Parser::parseBinOp() {
    BinOpType opType;
    if (peek().type == TokenType::ADD) opType = BinOpType::ADD;
    else if (peek().type == TokenType::SUBTRACT) opType = BinOpType::SUBTRACT;
    else if (peek().type == TokenType::MULTIPLY) opType = BinOpType::MULTIPLY;
    else opType = BinOpType::DIVIDE;
    get();

    auto stmt = std::make_unique<BinOpStmt>();
    stmt->line = peek().line;
    stmt->op = opType;

    if (peek().type != TokenType::IDENTIFIER) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected first operand.");
        return nullptr;
    }
    stmt->left = get().lexeme;

    if (!matchKeyword(TokenType::IN) && !matchKeyword(TokenType::AND)) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected 'and' or 'in' after first operand.");
        return nullptr;
    }

    if (peek().type != TokenType::IDENTIFIER) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected second operand.");
        return nullptr;
    }
    stmt->right = get().lexeme;

    expect(TokenType::STORE, "Expected 'store'");
    expect(TokenType::IN, "Expected 'in'");
    if (peek().type != TokenType::IDENTIFIER) {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected result variable after 'in'.");
        return nullptr;
    }
    stmt->result = get().lexeme;
    return stmt;
}

std::unique_ptr<Statement> Parser::parseIf() {
    auto stmt = std::make_unique<IfStmt>();
    stmt->line = peek().line;
    expect(TokenType::IF, "Expected 'if'");
    stmt->condition = parseExpression();
    expect(TokenType::THEN, "Expected 'then' after condition.");
    // For simplicity, parse one statement in then/else. Expand as needed.
    stmt->thenBranch.push_back(parseStatement());
    if (peek().type == TokenType::ELSE) {
        get();
        expect(TokenType::IF, "Expected 'if' after 'else' for else-if, or 'otherwise' for else.");
        stmt->elseIfBranches.push_back(parseStatement());
    }
    if (peek().type == TokenType::OTHERWISE) {
        get();
        stmt->elseBranch.push_back(parseStatement());
    }
    return stmt;
}

std::unique_ptr<Statement> Parser::parseRepeat() {
    auto stmt = std::make_unique<RepeatStmt>();
    stmt->line = peek().line;
    expect(TokenType::REPEAT, "Expected 'repeat'");
    if (peek().type == TokenType::FROM) {
        get();
        if (peek().type != TokenType::IDENTIFIER) {
            errors.push_back("Line " + std::to_string(peek().line) + ": Expected variable name after 'from'.");
            return nullptr;
        }
        stmt->varName = get().lexeme;
        expect(TokenType::ASSIGN, "Expected '=' after variable name.");
        stmt->start = parseExpression();
        expect(TokenType::TO, "Expected 'to'");
        stmt->end = parseExpression();
        expect(TokenType::JUMP, "Expected 'jump'");
        stmt->jump = parseExpression();
        // Parse loop body (for now, just one statement)
        stmt->body.push_back(parseStatement());
    } else if (peek().type == TokenType::UNTIL) {
        get();
        stmt->untilCondition = parseExpression();
        stmt->body.push_back(parseStatement());
    } else {
        errors.push_back("Line " + std::to_string(peek().line) + ": Expected 'from' or 'until' after 'repeat'.");
        return nullptr;
    }
    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    // For now, just parse primary or relop
    if (peek().type == TokenType::IDENTIFIER) {
        auto id = std::make_unique<Identifier>();
        id->name = get().lexeme;
        return id;
    }
    if (peek().type == TokenType::NUMBER) {
        auto num = std::make_unique<NumberLiteral>();
        num->value = get().lexeme;
        return num;
    }
    if (peek().type == TokenType::STRING) {
        auto str = std::make_unique<StringLiteral>();
        str->value = get().lexeme;
        return str;
    }
    if (peek().type == TokenType::REL_OP) {
        // Simple relop: a < b
        auto rel = std::make_unique<RelOpExpr>();
        rel->left = get().lexeme;
        rel->op = get().lexeme;
        rel->right = get().lexeme;
        return rel;
    }
    errors.push_back("Line " + std::to_string(peek().line) + ": Invalid expression.");
    return nullptr;
}

std::vector<std::string> Parser::getErrors() const {
    return errors;
}
