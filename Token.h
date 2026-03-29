#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    Number,
    Plus,
    Minus,
    Multiply,
    Divide,
    Assign,
    Identifier,
    Semicolon,
    LParen,
    RParen,
    EndOfFile,
    Print,
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

#endif