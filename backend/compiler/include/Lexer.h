#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>

enum class TokenType {
    // Keywords
    LET, BE, INPUT, OUTPUT,
    ADD, SUBTRACT, MULTIPLY, DIVIDE, STORE, IN, AND,
    IF, ELSE, OTHERWISE, THEN,
    REPEAT, FROM, TO, JUMP, UNTIL,
    // Operators
    ASSIGN, // '='
    REL_OP, // <, >, <=, >=, ==, !=
    // Literals
    IDENTIFIER, NUMBER, STRING,
    // Symbols
    END_OF_LINE,
    END_OF_FILE,
    // Error
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, const std::string& l, int ln, int col)
        : type(t), lexeme(l), line(ln), column(col) {}
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek() const;
    char get();
    void skipWhitespace();
    void skipComment();
    Token identifierOrKeyword();
    Token number();
    Token stringLiteral();
    Token relOp();
};

#endif // LEXER_H
