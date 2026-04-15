#ifndef TOKEN_H
#define TOKEN_H
#include <string>

enum class TokenType {
    Number, Plus, Minus, Multiply, Divide, Assign, Identifier, 
    Semicolon, LParen, RParen, LBrace, RBrace,
    If, While, Do, Print, EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType t = TokenType::EndOfFile, std::string v = "") : type(t), value(v) {}
};
#endif