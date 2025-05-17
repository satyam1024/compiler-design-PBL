#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "tokens.h"

class Lexer {
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string text;
    size_t pos;
    char current_char;

    void advance();
    void skip_whitespace();
    Token make_identifier_or_keyword();
    Token make_number();
    Token make_string_literal();
    Token get_next_token();
};

#endif