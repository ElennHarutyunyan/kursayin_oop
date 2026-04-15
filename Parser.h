#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Node.h"
#include <memory>

class Parser {
public:
    // Ensure this constructor exists
    Parser(Lexer lex); 
    
    // Ensure these methods are declared
    std::shared_ptr<StatementsNode> parse();
    std::shared_ptr<Node> statement();
    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> term();

private:
    Lexer lexer;
    Token currentToken;
    void eat(TokenType type);
};

#endif