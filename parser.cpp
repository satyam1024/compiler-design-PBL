// parser.cpp
#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::peek() { return pos < tokens.size() ? tokens[pos] : Token{TokenType::EOF_TOKEN, "", 0}; }
Token Parser::get() { return pos < tokens.size() ? tokens[pos++] : Token{TokenType::EOF_TOKEN, "", 0}; }
bool Parser::match(TokenType type) { if (peek().type == type) { get(); return true; } return false; }

ASTNodePtr Parser::parseProgram() {
    auto prog = std::make_shared<ProgramNode>();
    while (peek().type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt) prog->statements.push_back(stmt);
        else get(); // Skip unknown tokens
    }
    return prog;
}

ASTNodePtr Parser::parseStatement() {
    if (match(TokenType::LET)) return parseVarDecl();
    if (match(TokenType::PRINT)) return parsePrint();
    if (match(TokenType::INPUT)) return parseInput();
    if (match(TokenType::IF)) return parseIf();
    if (match(TokenType::FOR)) return parseFor();
    if (match(TokenType::LOOP)) return parseLoop();
    return nullptr;
}

ASTNodePtr Parser::parseVarDecl() {
    auto name = get();
    match(TokenType::BE_A);
    auto type = get();
    auto value = parseExpression();
    return std::make_shared<VarDeclNode>(name.value, type.value, value);
}

ASTNodePtr Parser::parsePrint() {
    auto expr = parseExpression();
    return std::make_shared<PrintNode>(expr);
}

ASTNodePtr Parser::parseInput() {
    auto var = get();
    return std::make_shared<InputNode>(var.value);
}

ASTNodePtr Parser::parseIf() {
    auto cond = parseExpression();
    match(TokenType::THEN);
    auto thenBlock = std::make_shared<BlockNode>();
    while (peek().type != TokenType::ELSE && peek().type != TokenType::END && peek().type != TokenType::EOF_TOKEN)
        thenBlock->statements.push_back(parseStatement());
    ASTNodePtr elseBlock = nullptr;
    if (match(TokenType::ELSE)) {
        elseBlock = std::make_shared<BlockNode>();
        while (peek().type != TokenType::END && peek().type != TokenType::EOF_TOKEN)
            std::dynamic_pointer_cast<BlockNode>(elseBlock)->statements.push_back(parseStatement());
    }
    match(TokenType::END);
    return std::make_shared<IfNode>(cond, thenBlock, elseBlock);
}

ASTNodePtr Parser::parseFor() {
    auto var = get();
    match(TokenType::IN);
    auto start = parseExpression();
    match(TokenType::TO);
    auto end = parseExpression();
    match(TokenType::THEN);
    auto block = std::make_shared<BlockNode>();
    while (peek().type != TokenType::END && peek().type != TokenType::EOF_TOKEN)
        block->statements.push_back(parseStatement());
    match(TokenType::END);
    return std::make_shared<ForNode>(var.value, start, end, block);
}

ASTNodePtr Parser::parseLoop() {
    auto times = parseExpression();
    match(TokenType::TIMES);
    match(TokenType::THEN);
    auto block = std::make_shared<BlockNode>();
    while (peek().type != TokenType::END && peek().type != TokenType::EOF_TOKEN)
        block->statements.push_back(parseStatement());
    match(TokenType::END);
    return std::make_shared<LoopNode>(times, block);
}

ASTNodePtr Parser::parseExpression() {
    // For simplicity, only literals and identifiers for now
    if (peek().type == TokenType::NUMBER) return std::make_shared<LiteralNode>(get().value);
    if (peek().type == TokenType::STRING) return std::make_shared<LiteralNode>(get().value);
    if (peek().type == TokenType::IDENTIFIER) return std::make_shared<IdentNode>(get().value);
    return nullptr;
}
