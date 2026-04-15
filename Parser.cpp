#include "Parser.h"
#include "Node.h"
#include <stdexcept>

Parser::Parser(Lexer lex) : lexer(lex), currentToken(lexer.getNextToken()) {}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        currentToken = lexer.getNextToken();
    } else {
        // Cast the enum to an int so to_string can handle it
        throw std::runtime_error("Unexpected token: expected type " + std::to_string(static_cast<int>(type)));
    }
}

std::shared_ptr<Node> Parser::term() {
    if (currentToken.type == TokenType::Number) {
        double val = std::stod(currentToken.value);
        eat(TokenType::Number);
        return std::make_shared<NumberNode>(val);
    } else if (currentToken.type == TokenType::Identifier) {
        std::string name = currentToken.value;
        eat(TokenType::Identifier);
        return std::make_shared<VarNode>(name);
    } else if (currentToken.type == TokenType::LParen) {
        eat(TokenType::LParen);
        auto node = expr();
        eat(TokenType::RParen);
        return node;
    }
    return nullptr;
}

std::shared_ptr<Node> Parser::expr() {
    auto node = term();
    while (currentToken.type == TokenType::Plus || currentToken.type == TokenType::Minus) {
        char op = (currentToken.type == TokenType::Plus) ? '+' : '-';
        eat(currentToken.type);
        auto right = term();
        node = std::make_shared<BinOpNode>(op, node, right);
    }
    return node;
}

std::shared_ptr<Node> Parser::statement() {
    if (currentToken.type == TokenType::Print) {
        eat(TokenType::Print);
        auto val = expr();
        eat(TokenType::Semicolon);
        return std::make_shared<PrintNode>(val);
    } 
    
    else if (currentToken.type == TokenType::If || currentToken.type == TokenType::While) {
        bool isWhile = (currentToken.type == TokenType::While);
        eat(currentToken.type); // eats 'if' or 'while'
        
        eat(TokenType::LParen);
        auto cond = expr();
        eat(TokenType::RParen);
        
        eat(TokenType::LBrace);
        auto body = std::make_shared<StatementsNode>(); // The container
        
        while (currentToken.type != TokenType::RBrace && currentToken.type != TokenType::EndOfFile) {
            auto stmt = statement();
            if (stmt) {
                body->statements.push_back(stmt); // <--- Are you pushing into 'body'?
            }
        }
        eat(TokenType::RBrace);
        
        if (isWhile) return std::make_shared<WhileNode>(cond, body);
        return std::make_shared<IfNode>(cond, body); // <--- Passing 'body' here
    }
    
    else if (currentToken.type == TokenType::Identifier) {
        std::string name = currentToken.value;
        eat(TokenType::Identifier);
        eat(TokenType::Assign);
        auto val = expr();
        eat(TokenType::Semicolon);
        return std::make_shared<AssignNode>(name, val);
    }
    
    return nullptr;
}

std::shared_ptr<StatementsNode> Parser::parse() {
    auto root = std::make_shared<StatementsNode>();
    while (currentToken.type != TokenType::EndOfFile) {
        auto stmt = statement();
        if (stmt) {
            root->statements.push_back(stmt);
        }
    }
    return root;
}