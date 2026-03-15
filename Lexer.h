#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
private:
    std::string input;
    size_t pos;
    char currentChar;

    // Moves the pointer forward in the input string
    void advance();
    
    // Skips spaces, tabs, and newlines
    void skipWhitespace();
    
    // Helper to extract multi-digit numbers
    Token number();
    
    // Helper to extract variable names or keywords like 'print'
    Token identifier();

public:
    Lexer(const std::string &text);
    
    // The main method used by the Parser to get the next piece of code
    Token getNextToken();
};

#endif // LEXER_H