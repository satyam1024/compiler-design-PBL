#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>

enum class TokenType {
    LET, BE, INPUT, OUTPUT,
    ADD, SUBTRACT, MULTIPLY, DIVIDE, STORE, IN, AND,
    IF, ELSE, OTHERWISE, THEN,
    REPEAT, FROM, TO, JUMP, UNTIL,
    ASSIGN, 
    REL_OP, 
    IDENTIFIER, NUMBER, STRING,
    END_OF_LINE,
    END_OF_FILE,
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

#endif 
