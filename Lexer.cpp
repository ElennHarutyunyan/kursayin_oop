#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string &text) : input(text), pos(0) {
    currentChar = input.empty() ? '\0' : input[pos];
}

void Lexer::advance() {
    pos++;
    currentChar = (pos < input.size()) ? input[pos] : '\0';
}

void Lexer::skipWhitespace() {
    while (currentChar && isspace(currentChar)) {
        advance();
    }
}

Token Lexer::number() {
    std::string result;
    while (currentChar && isdigit(currentChar)) {
        result += currentChar;
        advance();
    }
    // If you want to support decimals (double), you could add logic for '.' here
    return Token(TokenType::Number, result);
}

Token Lexer::identifier() {
    std::string result;
    // Identifiers can be letters, numbers (after the first char), or underscores
    while (currentChar && (isalnum(currentChar) || currentChar == '_')) {
        result += currentChar;
        advance();
    }
    
    // Check if the identifier is a reserved keyword
    if (result == "print") {
        return Token(TokenType::Print, result);
    }
    
    return Token(TokenType::Identifier, result);
}

Token Lexer::getNextToken() {
    while (currentChar != '\0') {
        if (isspace(currentChar)) {
            skipWhitespace();
            continue;
        }

        if (isdigit(currentChar)) {
            return number();
        }

        if (isalpha(currentChar) || currentChar == '_') {
            return identifier();
        }

        // Handle single-character operators and symbols
        switch (currentChar) {
            case '+':
                advance();
                return Token(TokenType::Plus, "+");
            case '-':
                advance();
                return Token(TokenType::Minus, "-");
            case '*':
                advance();
                return Token(TokenType::Multiply, "*");
            case '/':
                advance();
                return Token(TokenType::Divide, "/");
            case '=':
                advance();
                return Token(TokenType::Assign, "=");
            case ';':
                advance();
                return Token(TokenType::Semicolon, ";");
            case '(':
                advance();
                return Token(TokenType::LParen, "(");
            case ')':
                advance();
                return Token(TokenType::RParen, ")");
            default:
                // If we hit an unknown character, skip it or handle as error
                advance(); 
        }
    }

    return Token(TokenType::EndOfFile);
}