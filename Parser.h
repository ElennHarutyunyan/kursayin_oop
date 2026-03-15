#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Node.h"  // <--- CRITICAL: This fixes the 'Node' not declared errors
#include <memory>
#include <vector>

class Parser {
private:
    Lexer lexer;
    Token currentToken;

    void eat(TokenType type);
    std::shared_ptr<Node> factor();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> statement();

public:
    Parser(Lexer l);
    std::shared_ptr<StatementsNode> parse();
};

#endif