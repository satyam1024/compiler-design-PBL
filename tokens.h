#ifndef TOKENS_H
#define TOKENS_H

#include <string>
#include<unordered_map>
using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_STRING, T_CHAR, T_CONST,
    T_SIZEOF, T_LEN,
    T_IF, T_ELSEIF, T_ELSE,
    T_ITER, T_BREAK, T_CONTINUE,
    T_SWITCH, T_CONDITION, T_BASECON,
    T_RESPONSE, T_INPUT, T_OUTPUT,
    T_IDENTIFIER, T_NUMBER, T_STRING_LITERAL, T_CHAR_LITERAL,
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_SEMI, T_COMMA,
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_ASSIGN,
    T_EOF, T_UNKNOWN
};

extern unordered_map<int, string> tokenTypeMap;
struct Token {
    TokenType type;
    string value;
    Token(TokenType type, const string& value) : type(type), value(value) {}
};

#endif