#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Enum defining all possible types of pieces in your code
enum class TokenType {
    Number,      // e.g., "42", "3.14"
    Plus,        // "+"
    Minus,       // "-"
    Multiply,    // "*"
    Divide,      // "/"
    Assign,      // "="
    Identifier,  // e.g., "x", "result"
    Semicolon,   // ";"
    LParen,      // "("
    RParen,      // ")"
    EndOfFile,   // Special marker for the end of the input
    Print,       // The "print" keyword
};

struct Token {
    TokenType type;
    std::string value;

    // Constructor to easily create tokens with an optional string value
    Token(TokenType t, std::string v = "") : type(t), value(v) {}
};

#endif // TOKEN_H