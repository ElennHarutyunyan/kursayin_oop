#include "Parser.h"

// --- Helper Functions Implementation ---

Token Parser::peek() const { return tokens[pos]; }
Token Parser::previous() const { return tokens[pos - 1]; }
bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
bool Parser::check(TokenType type) const { return !isAtEnd() && peek().type == type; }

Token Parser::advance() {
    if (!isAtEnd()) pos++;
    return previous();
}

bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Parser Error at line " + std::to_string(peek().line) + ": " + message);
}

// --- Parsing Logic ---

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::declaration() {
    if (match({TokenType::Int, TokenType::Float})) {
        std::string type = previous().value;
        Token name = consume(TokenType::Identifier, "Expect variable name.");
        std::unique_ptr<ExprNode> initializer = nullptr;
        if (match({TokenType::Assign})) {
            initializer = expression();
        }
        consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
        return std::make_unique<DeclarationNode>(type, name.value, false, std::move(initializer));
    }
    return statement();
}

std::unique_ptr<ASTNode> Parser::statement() {
    if (match({TokenType::If})) return ifStatement();
    if (match({TokenType::While})) return whileStatement();
    if (check(TokenType::LBrace)) return std::make_unique<AssignmentNode>("block", nullptr); // Placeholder for block logic

    // Simple Assignment
    if (check(TokenType::Identifier)) {
        Token name = advance();
        consume(TokenType::Assign, "Expect '='.");
        auto expr = expression();
        consume(TokenType::Semicolon, "Expect ';' after assignment.");
        return std::make_unique<AssignmentNode>(name.value, std::move(expr));
    }
    
    throw std::runtime_error("Unknown statement at line " + std::to_string(peek().line));
}

std::unique_ptr<ASTNode> Parser::ifStatement() {
    consume(TokenType::LParen, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RParen, "Expect ')' after if condition.");

    auto thenBranch = block();
    std::vector<std::unique_ptr<ASTNode>> elseBranch;
    if (match({TokenType::Else})) {
        elseBranch = block();
    }

    auto node = std::make_unique<IfNode>();
    node->condition = std::move(condition);
    node->thenBlock = std::move(thenBranch);
    node->elseBlock = std::move(elseBranch);
    return node;
}

std::unique_ptr<ASTNode> Parser::whileStatement() {
    consume(TokenType::LParen, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RParen, "Expect ')' after while condition.");
    auto body = block();

    auto node = std::make_unique<WhileNode>();
    node->condition = std::move(condition);
    node->body = std::move(body);
    return node;
}

std::vector<std::unique_ptr<ASTNode>> Parser::block() {
    consume(TokenType::LBrace, "Expect '{' before block.");
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RBrace, "Expect '}' after block.");
    return statements;
}

// --- Expression Parsing (Recursive Descent) ---

std::unique_ptr<ExprNode> Parser::expression() {
    return equality();
}

std::unique_ptr<ExprNode> Parser::equality() {
    auto expr = comparison();
    while (match({TokenType::Eq, TokenType::Neq})) {
        std::string op = previous().value;
        auto right = comparison();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::comparison() {
    auto expr = term();
    while (match({TokenType::Gt, TokenType::Geq, TokenType::Lt, TokenType::Leq})) {
        std::string op = previous().value;
        auto right = term();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::term() {
    auto expr = factor();
    while (match({TokenType::Plus, TokenType::Minus})) {
        std::string op = previous().value;
        auto right = factor();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::factor() {
    auto expr = primary();
    while (match({TokenType::Star, TokenType::Slash})) {
        std::string op = previous().value;
        auto right = primary();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::primary() {
    if (match({TokenType::Number})) return std::make_unique<IntLiteralNode>(std::stoi(previous().value));
    if (match({TokenType::Identifier})) return std::make_unique<VariableNode>(previous().value);
    
    if (match({TokenType::LParen})) {
        auto expr = expression();
        consume(TokenType::RParen, "Expect ')' after expression.");
        return expr;
    }

    throw std::runtime_error("Expect expression at line " + std::to_string(peek().line));
}