#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "tokens.h"
using namespace std;

class Lexer {
public:
    Lexer(const string& input);
    vector<Token> tokenize();

private:
    string text;
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