// lexer.h
#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

enum class TokenType {
    LET, BE_A, INT, IDENTIFIER, NUMBER, IF, THEN, ELSE, END, FOR, IN, TO, LOOP, TIMES, INPUT, PRINT,
    STRING, ASSIGN, EOL, UNKNOWN, EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

class Lexer {
public:
    Lexer(const std::string& src);
    std::vector<Token> tokenize();
private:
    std::string src;
    size_t pos;
    int line;
    std::unordered_map<std::string, TokenType> keywords;
    char peek();
    char get();
    void skipWhitespace();
    Token nextToken();
};
