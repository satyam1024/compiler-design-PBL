#include "lexer.h"
#include <unordered_map>
#include<iostream>
#include <cctype>
using namespace std;

unordered_map<string, TokenType> keywords = {
    {"vari", T_INT}, {"varf", T_FLOAT}, {"vars", T_STRING}, {"varc", T_CHAR}, {"constant", T_CONST},
    {"sizeof", T_SIZEOF}, {"len", T_LEN},
    {"if", T_IF}, {"else", T_ELSE}, {"else if", T_ELSEIF},
    {"iter", T_ITER}, {"break", T_BREAK}, {"continue", T_CONTINUE},
    {"switch", T_SWITCH}, {"condition", T_CONDITION}, {"basecon", T_BASECON},
    {"response", T_RESPONSE}, {"input", T_INPUT}, {"output", T_OUTPUT}
};

unordered_map<int, string> tokenTypeMap = {
    {T_INT, "T_INT"},
    {T_FLOAT, "T_FLOAT"},
    {T_STRING, "T_STRING"},
    {T_CHAR, "T_CHAR"},
    {T_CONST, "T_CONST"},
    {T_SIZEOF, "T_SIZEOF"},
    {T_LEN, "T_LEN"},
    {T_IF, "T_IF"},
    {T_ELSEIF, "T_ELSEIF"},
    {T_ELSE, "T_ELSE"},
    {T_ITER, "T_ITER"},
    {T_BREAK, "T_BREAK"},
    {T_CONTINUE, "T_CONTINUE"},
    {T_SWITCH, "T_SWITCH"},
    {T_CONDITION, "T_CONDITION"},
    {T_BASECON, "T_BASECON"},
    {T_RESPONSE, "T_RESPONSE"},
    {T_INPUT, "T_INPUT"},
    {T_OUTPUT, "T_OUTPUT"},
    {T_IDENTIFIER, "T_IDENTIFIER"},
    {T_NUMBER, "T_NUMBER"},
    {T_STRING_LITERAL, "T_STRING_LITERAL"},
    {T_CHAR_LITERAL, "T_CHAR_LITERAL"},
    {T_LPAREN, "T_LPAREN"},
    {T_RPAREN, "T_RPAREN"},
    {T_LBRACE, "T_LBRACE"},
    {T_RBRACE, "T_RBRACE"},
    {T_SEMI, "T_SEMI"},
    {T_COMMA, "T_COMMA"},
    {T_PLUS, "T_PLUS"},
    {T_MINUS, "T_MINUS"},
    {T_MUL, "T_MUL"},
    {T_DIV, "T_DIV"},
    {T_ASSIGN, "T_ASSIGN"},
    {T_EOF, "T_EOF"},
    {T_UNKNOWN, "T_UNKNOWN"}
};


Lexer::Lexer(const string& input) : text(input), pos(0) {
    current_char = text[pos];
}

void Lexer::advance() {
    pos++;
    current_char = pos < text.size() ? text[pos] : '\0';
}

void Lexer::skip_whitespace() {
    while (isspace(current_char)) advance();
}

Token Lexer::make_identifier_or_keyword() {
    string result;
    while (isalnum(current_char) || current_char == '_') {
        result += current_char;
        advance();
    }
    if (result == "else" && text.substr(pos, 4) == " if") {
        advance(); advance(); advance(); advance();
        return Token(T_ELSEIF, "else if");
    }
    if (keywords.count(result)) return Token(keywords[result], result);
    return Token(T_IDENTIFIER, result);
}

Token Lexer::make_number() {
    string result;
    int countofdot=0;
    while (isdigit(current_char) || current_char == '.' ) 
    {
        if(current_char == '.')countofdot++;
        if(countofdot>=1)
        {
            while(isdigit(current_char) || current_char == '.')
            {
                result += current_char;
                advance();
            }
             return Token(T_UNKNOWN, result);
        }
        result += current_char;
        advance();
    }
    return Token(T_NUMBER, result);
}

Token Lexer::make_string_literal() {
    string result;
    advance();
    while (current_char != '"' && current_char != '\0') {
        result += current_char;
        advance();
    }
    advance();
    return Token(T_STRING_LITERAL, result);
}

Token Lexer::get_next_token() {
    skip_whitespace();
    if (current_char == '\0') return Token(T_EOF, "");

    if (isalpha(current_char) || current_char == '_') return make_identifier_or_keyword();
    if (isdigit(current_char)) return make_number();
    if (current_char == '"') return make_string_literal();

    switch (current_char) {
        case '+': advance(); return Token(T_PLUS, "+");
        case '-': advance(); return Token(T_MINUS, "-");
        case '*': advance(); return Token(T_MUL, "*");
        case '/': advance(); return Token(T_DIV, "/");
        case '=': advance(); return Token(T_ASSIGN, "=");
        case '(': advance(); return Token(T_LPAREN, "(");
        case ')': advance(); return Token(T_RPAREN, ")");
        case '{': advance(); return Token(T_LBRACE, "{");
        case '}': advance(); return Token(T_RBRACE, "}");
        case ';': advance(); return Token(T_SEMI, ";");
        case ',': advance(); return Token(T_COMMA, ",");
        default: {
            string unknown(1, current_char);
            advance();
            return Token(T_UNKNOWN, unknown);
        }
    }
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    Token tok = get_next_token();
    while (tok.type != T_EOF) {
        tokens.push_back(tok);
        tok = get_next_token();
    }
    return tokens;
}

