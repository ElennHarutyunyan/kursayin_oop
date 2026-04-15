#ifndef LEXER_H
#define LEXER_H
#include "Token.h"
#include <string>

class Lexer {
private:
    std::string input;
    size_t pos;
    char currentChar;
public:
    Lexer(const std::string &text);
    void advance();
    void skipWhitespace();
    Token number();
    Token identifier();
    Token getNextToken();
};
#endif