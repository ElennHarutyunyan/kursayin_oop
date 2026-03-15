#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer l) : lexer(l), currentToken(lexer.getNextToken()) {}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        currentToken = lexer.getNextToken();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

std::shared_ptr<Node> Parser::factor() {
    Token tok = currentToken;
    if (tok.type == TokenType::Number) {
        eat(TokenType::Number);
        return std::make_shared<NumberNode>(std::stod(tok.value));
    } else if (tok.type == TokenType::Identifier) {
        eat(TokenType::Identifier);
        return std::make_shared<VarNode>(tok.value);
    } else if (tok.type == TokenType::LParen) {
        eat(TokenType::LParen);
        auto node = expr();
        eat(TokenType::RParen);
        return node;
    }
    throw std::runtime_error("Invalid factor");
}

std::shared_ptr<Node> Parser::term() {
    auto node = factor();
    while (currentToken.type == TokenType::Multiply || currentToken.type == TokenType::Divide) {
        char op = (currentToken.type == TokenType::Multiply) ? '*' : '/';
        eat(currentToken.type);
        node = std::make_shared<BinOpNode>(node, op, factor());
    }
    return node;
}

std::shared_ptr<Node> Parser::expr() {
    auto node = term();
    while (currentToken.type == TokenType::Plus || currentToken.type == TokenType::Minus) {
        char op = (currentToken.type == TokenType::Plus) ? '+' : '-';
        eat(currentToken.type);
        node = std::make_shared<BinOpNode>(node, op, term());
    }
    return node;
}

std::shared_ptr<Node> Parser::statement() {
    if (currentToken.type == TokenType::Print) {
        eat(TokenType::Print);
        auto exprNode = expr();
        eat(TokenType::Semicolon);
        return std::make_shared<PrintNode>(exprNode);
    } else if (currentToken.type == TokenType::Identifier) {
        std::string varName = currentToken.value;
        eat(TokenType::Identifier);
        if (currentToken.type == TokenType::Assign) {
            eat(TokenType::Assign);
            auto valueNode = expr();
            eat(TokenType::Semicolon);
            return std::make_shared<AssignNode>(varName, valueNode);
        }
    }
    return nullptr;
}

std::shared_ptr<StatementsNode> Parser::parse() {
    auto root = std::make_shared<StatementsNode>();
    while (currentToken.type != TokenType::EndOfFile) {
        auto stmt = statement();
        if (stmt) root->statements.push_back(stmt);
    }
    return root;
}