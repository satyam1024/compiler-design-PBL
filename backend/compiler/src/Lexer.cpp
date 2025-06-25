#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::LET},
    {"be", TokenType::BE},
    {"input", TokenType::INPUT},
    {"output", TokenType::OUTPUT},
    {"add", TokenType::ADD},
    {"subtract", TokenType::SUBTRACT},
    {"multiply", TokenType::MULTIPLY},
    {"divide", TokenType::DIVIDE},
    {"store", TokenType::STORE},
    {"in", TokenType::IN},
    {"and", TokenType::AND},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"otherwise", TokenType::OTHERWISE},
    {"then", TokenType::THEN},
    {"repeat", TokenType::REPEAT},
    {"from", TokenType::FROM},
    {"to", TokenType::TO},
    {"jump", TokenType::JUMP},
    {"until", TokenType::UNTIL}
};

Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::get() {
    if (pos >= source.length()) return '\0';
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) get();
}

Token Lexer::identifierOrKeyword() {
    int startCol = column;
    std::string lexeme;
    while (std::isalnum(peek()) || peek() == '_') {
        lexeme += get();
    }
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, line, startCol);
    }
    return Token(TokenType::IDENTIFIER, lexeme, line, startCol);
}

Token Lexer::number() {
    int startCol = column;
    std::string lexeme;
    bool hasDot = false;
    while (std::isdigit(peek()) || (!hasDot && peek() == '.')) {
        if (peek() == '.') hasDot = true;
        lexeme += get();
    }
    return Token(TokenType::NUMBER, lexeme, line, startCol);
}

Token Lexer::stringLiteral() {
    int startCol = column;
    std::string lexeme;
    get(); 
    while (peek() != '"' && peek() != '\0') {
        lexeme += get();
    }
    if (peek() == '"') get(); 
    else return Token(TokenType::INVALID, lexeme, line, startCol); 
    return Token(TokenType::STRING, lexeme, line, startCol);
}

Token Lexer::relOp() {
    int startCol = column;
    char c = get();
    std::string lexeme(1, c);

    if (peek() == '=' && (c == '<' || c == '>' || c == '=' || c == '!')) {
        lexeme += get();
        return Token(TokenType::REL_OP, lexeme, line, startCol);
    }
    else if (c == '<' || c == '>') {
        return Token(TokenType::REL_OP, lexeme, line, startCol);
    }
    return Token(TokenType::INVALID, lexeme, line, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        skipWhitespace();

        char c = peek();
        if (c == '\0') {
            tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
            break;
        }

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(identifierOrKeyword());
        }
        else if (std::isdigit(c)) {
            tokens.push_back(number());
        }
        else if (c == '"') {
            tokens.push_back(stringLiteral());
        }
        else if (c == '=')
        {
            get();
            if (peek() == '=')
            {
                get();
                tokens.push_back(Token(TokenType::REL_OP, "==", line, column));
            }
            else
            {
                tokens.push_back(Token(TokenType::ASSIGN, "=", line, column));
            }
            }
            else if (c == '<' || c == '>' || c == '!')
            {
                tokens.push_back(relOp());
            }

        else if (c == '\n') {
            get();
            tokens.emplace_back(TokenType::END_OF_LINE, "\\n", line - 1, 1);
        }
        else {
            get(); 
            tokens.emplace_back(TokenType::INVALID, std::string(1, c), line, column);
        }
    }
    return tokens;
}
