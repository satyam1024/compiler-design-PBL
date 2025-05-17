#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::current() {
    return pos < tokens.size() ? tokens[pos] : Token(T_EOF, "");
}

Token Parser::advance() {
    return pos < tokens.size() ? tokens[pos++] : Token(T_EOF, "");
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(TokenType type, const std::string& msg) {
    if (match(type)) return true;
    std::cerr << "Syntax error near token: " << current().value << " (" << msg << ")\n";
    return false;
}

void Parser::parse() {
    while (current().type != T_EOF) {
        statement();
    }
}

void Parser::statement() {
    Token tok = current();

    switch (tok.type) {
        case T_INT:
        case T_FLOAT:
        case T_STRING:
        case T_CHAR:
        case T_CONST:
            declaration();
            break;
        case T_OUTPUT:
            outputStatement();
            break;
        default:
            std::cerr << "Syntax error near token: " << tok.value << "\n";
            advance();
    }
}

void Parser::declaration() {
    Token typeToken = advance(); // vari, varf, etc.

    bool isConst = (typeToken.type == T_CONST);
    if (isConst) {
        if (!match(T_INT) && !match(T_FLOAT) && !match(T_STRING) && !match(T_CHAR)) {
            std::cerr << "Expected data type after 'constant'\n";
            return;
        }
        typeToken = tokens[pos - 1];
    }

    if (!expect(T_IDENTIFIER, "Expected variable name")) return;
    Token identifier = tokens[pos - 1];

    if (symbolTable.count(identifier.value)) {
        std::cerr << "Semantic error: Variable '" << identifier.value << "' already declared.\n";
        return;
    }

    if (match(T_ASSIGN)) {
        if (!match(T_NUMBER) && !match(T_STRING_LITERAL)) {
            std::cerr << "Expected a value after '='\n";
        }
    }

    expect(T_SEMI, "Expected ';' after declaration");

    // ✅ Add to symbol table
    symbolTable[identifier.value] = {typeToken.value, isConst};

    std::cout << "Valid declaration: " << identifier.value << " of type " << typeToken.value
              << (isConst ? " (const)" : "") << "\n";
}

void Parser::outputStatement() {
    advance(); // consume 'output'

    if (!expect(T_IDENTIFIER, "Expected variable after output")) return;
    Token identifier = tokens[pos - 1];

    // ✅ Semantic check: must be declared
    if (!symbolTable.count(identifier.value)) {
        std::cerr << "Semantic error: Variable '" << identifier.value << "' not declared.\n";
        return;
    }

    expect(T_SEMI, "Expected ';' after output");
    std::cout << "Valid output: " << identifier.value << "\n";
}
