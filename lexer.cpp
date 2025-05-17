#include "lexer.h"
#include <unordered_map>
#include <cctype>

std::unordered_map<std::string, TokenType> keywords = {
    {"vari", T_INT}, {"varf", T_FLOAT}, {"vars", T_STRING}, {"varc", T_CHAR}, {"constant", T_CONST},
    {"sizeof", T_SIZEOF}, {"len", T_LEN},
    {"if", T_IF}, {"else", T_ELSE}, {"else if", T_ELSEIF},
    {"iter", T_ITER}, {"break", T_BREAK}, {"continue", T_CONTINUE},
    {"switch", T_SWITCH}, {"condition", T_CONDITION}, {"basecon", T_BASECON},
    {"response", T_RESPONSE}, {"input", T_INPUT}, {"output", T_OUTPUT}
};

Lexer::Lexer(const std::string& input) : text(input), pos(0) {
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
    std::string result;
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
    std::string result;
    while (isdigit(current_char) || current_char == '.') {
        result += current_char;
        advance();
    }
    return Token(T_NUMBER, result);
}

Token Lexer::make_string_literal() {
    std::string result;
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
            std::string unknown(1, current_char);
            advance();
            return Token(T_UNKNOWN, unknown);
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token tok = get_next_token();
    while (tok.type != T_EOF) {
        tokens.push_back(tok);
        tok = get_next_token();
    }
    return tokens;
}

