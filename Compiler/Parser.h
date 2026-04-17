#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <stdexcept>
#include "Lexer.h"
#include "ASTNodes.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;

    // Helper functions
    Token peek() const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(std::vector<TokenType> types);
    Token consume(TokenType type, const std::string& message);

    // Parsing methods
    std::unique_ptr<ASTNode> declaration();
    std::unique_ptr<ASTNode> statement();
    std::unique_ptr<ASTNode> ifStatement();
    std::unique_ptr<ASTNode> whileStatement();
    std::vector<std::unique_ptr<ASTNode>> block();
    
    std::unique_ptr<ExprNode> expression();
    std::unique_ptr<ExprNode> assignment();
    std::unique_ptr<ExprNode> equality();
    std::unique_ptr<ExprNode> comparison();
    std::unique_ptr<ExprNode> term();
    std::unique_ptr<ExprNode> factor();
    std::unique_ptr<ExprNode> primary();

public:
    Parser(const std::vector<Token>& t) : tokens(t), pos(0) {}
    std::vector<std::unique_ptr<ASTNode>> parse();
};

#endif