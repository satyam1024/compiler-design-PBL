// lexer.cpp
#include "lexer.h"

Lexer::Lexer(const std::string& src) : src(src), pos(0), line(1) {
    keywords = {
        {"let", TokenType::LET}, {"be_a", TokenType::BE_A}, {"int", TokenType::INT},
        {"if", TokenType::IF}, {"then", TokenType::THEN}, {"else", TokenType::ELSE}, {"end", TokenType::END},
        {"for", TokenType::FOR}, {"in", TokenType::IN}, {"to", TokenType::TO},
        {"loop", TokenType::LOOP}, {"times", TokenType::TIMES},
        {"input", TokenType::INPUT}, {"print", TokenType::PRINT}
    };
}

char Lexer::peek() { return pos < src.size() ? src[pos] : '\0'; }
char Lexer::get() { return pos < src.size() ? src[pos++] : '\0'; }

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        if (peek() == '\n') ++line;
        get();
    }
}

Token Lexer::nextToken() {
    skipWhitespace();
    if (pos >= src.size()) return {TokenType::EOF_TOKEN, "", line};

    if (isalpha(peek())) {
        std::string word;
        while (isalnum(peek()) || peek() == '_') word += get();
        if (keywords.count(word)) return {keywords[word], word, line};
        return {TokenType::IDENTIFIER, word, line};
    }
    if (isdigit(peek())) {
        std::string num;
        while (isdigit(peek())) num += get();
        return {TokenType::NUMBER, num, line};
    }
    if (peek() == '"') {
        get(); // skip "
        std::string str;
        while (peek() != '"' && peek() != '\0') str += get();
        get(); // skip closing "
        return {TokenType::STRING, str, line};
    }
    if (peek() == '=') { get(); return {TokenType::ASSIGN, "=", line}; }
    if (peek() == '\n') { get(); return {TokenType::EOL, "", line}; }

    // Unknown character
    char unknown = get();
    return {TokenType::UNKNOWN, std::string(1, unknown), line};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token t;
    do {
        t = nextToken();
        tokens.push_back(t);
    } while (t.type != TokenType::EOF_TOKEN);
    return tokens;
}
