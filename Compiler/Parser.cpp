#include "Parser.h"

Token Parser::peek() const {
    return tokens[pos];
}

Token Parser::previous() const {
    return tokens[pos - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        pos++;
    }
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}

bool Parser::isTypeToken(TokenType type) const {
    return type == TokenType::Int || type == TokenType::Void ||
           type == TokenType::Float || type == TokenType::Double ||
           type == TokenType::Char || type == TokenType::Identifier;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> nodes;
    while (!isAtEnd()) {
        nodes.push_back(prog_parser());
    }
    return nodes;
}

std::unique_ptr<ASTNode> Parser::prog_parser() {
    if (match({TokenType::Class, TokenType::Struct})) {
        auto node = std::make_unique<ClassNode>();
        node->isStruct = (previous().type == TokenType::Struct);
        node->name = consume(TokenType::Identifier, "Expected class/struct name").value;
        consume(TokenType::LBrace, "Expected {");
        while (!check(TokenType::RBrace) && !isAtEnd()) {
            advance();
        }
        consume(TokenType::RBrace, "Expected }");
        if (check(TokenType::Semicolon)) {
            advance();
        }
        return node;
    }

    bool isStatic = match({TokenType::Static});
    if (!isTypeToken(peek().type)) {
        throw std::runtime_error("Top-level declaration must start with a type");
    }
    std::string type = advance().value;
    std::string name = consume(TokenType::Identifier, "Expected identifier").value;

    if (check(TokenType::LParen)) {
        return func_parser(type, name);
    }
    return declaration(type, name, isStatic, true);
}

std::unique_ptr<ASTNode> Parser::func_parser(const std::string& retType, const std::string& name) {
    auto func = std::make_unique<FunctionNode>();
    func->returnType = retType;
    func->name = name;

    consume(TokenType::LParen, "Expected (");
    while (!check(TokenType::RParen) && !isAtEnd()) {
        if (!isTypeToken(peek().type)) {
            throw std::runtime_error("Expected parameter type");
        }
        std::string pType = advance().value;
        std::string pName = consume(TokenType::Identifier, "Expected parameter name").value;
        func->params.push_back({pType, pName});
        if (check(TokenType::Comma)) {
            advance();
        } else {
            break;
        }
    }
    consume(TokenType::RParen, "Expected )");

    func->body = func_code();
    return func;
}

std::vector<std::unique_ptr<ASTNode>> Parser::func_code() {
    consume(TokenType::LBrace, "Expected {");
    std::vector<std::unique_ptr<ASTNode>> stmts;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        stmts.push_back(statement());
    }
    consume(TokenType::RBrace, "Expected }");
    return stmts;
}

std::unique_ptr<ASTNode> Parser::declaration(const std::string& type, const std::string& name, bool isStatic, bool isGlobal) {
    std::unique_ptr<ExprNode> init = nullptr;
    if (match({TokenType::Assign})) {
        init = expression();
    }
    consume(TokenType::Semicolon, "Expected ; after declaration");
    return std::make_unique<DeclarationNode>(type, name, isStatic, isGlobal, std::move(init));
}

std::unique_ptr<ASTNode> Parser::statement() {
    if (check(TokenType::If)) {
        advance();
        return ifStatement();
    }
    if (check(TokenType::While)) {
        advance();
        return whileStatement();
    }
    if (check(TokenType::For)) {
        advance();
        return forStatement();
    }
    if (check(TokenType::Switch)) {
        advance();
        return switchStatement();
    }
    if (check(TokenType::Return)) {
        advance();
        return returnStatement();
    }
    if (check(TokenType::Break)) {
        advance();
        return breakStatement();
    }
    if (check(TokenType::Continue)) {
        advance();
        return continueStatement();
    }
    if (check(TokenType::Static)) {
        advance();
        std::string type = consume(TokenType::Int, "Expected type after static").value;
        std::string name = consume(TokenType::Identifier, "Expected name").value;
        return declaration(type, name, true);
    }
    if (check(TokenType::Int) || check(TokenType::Void) || check(TokenType::Float) ||
        check(TokenType::Double) || check(TokenType::Char)) {
        std::string type = advance().value;
        std::string name = consume(TokenType::Identifier, "Expected identifier").value;
        return declaration(type, name, false);
    }

    auto expr = expression();
    consume(TokenType::Semicolon, "Expected ; after expression");
    return expr;
}

std::unique_ptr<ASTNode> Parser::switchStatement() {
    auto sw = std::make_unique<SwitchNode>();
    consume(TokenType::LParen, "Expected ( after switch");
    sw->condition = expression();
    consume(TokenType::RParen, "Expected )");
    consume(TokenType::LBrace, "Expected {");

    switchDepth++;
    SwitchNode::Section* currentSection = nullptr;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        if (match({TokenType::Case})) {
            int caseValue = std::stoi(consume(TokenType::Number, "Expected case integer").value);
            consume(TokenType::Colon, "Expected :");
            sw->orderedSections.push_back({false, caseValue, {}});
            currentSection = &sw->orderedSections.back();
            continue;
        }
        if (match({TokenType::Default})) {
            consume(TokenType::Colon, "Expected :");
            sw->orderedSections.push_back({true, 0, {}});
            currentSection = &sw->orderedSections.back();
            continue;
        }
        if (currentSection == nullptr) {
            throw std::runtime_error("Switch statement requires case/default label before statements");
        }
        currentSection->statements.push_back(statement());
    }
    switchDepth--;

    consume(TokenType::RBrace, "Expected } after switch body");
    return sw;
}

std::unique_ptr<ASTNode> Parser::returnStatement() {
    std::unique_ptr<ExprNode> expr = nullptr;
    if (!check(TokenType::Semicolon)) {
        expr = expression();
    }
    consume(TokenType::Semicolon, "Expected ; after return");
    return std::make_unique<ReturnNode>(std::move(expr));
}

std::unique_ptr<ASTNode> Parser::breakStatement() {
    if (switchDepth == 0 && loopDepth == 0) {
        throw std::runtime_error("Invalid 'break': only allowed inside switch or loop");
    }
    consume(TokenType::Semicolon, "Expected ; after break");
    return std::make_unique<BreakNode>();
}

std::unique_ptr<ASTNode> Parser::continueStatement() {
    if (loopDepth == 0) {
        throw std::runtime_error("Invalid 'continue': only allowed inside loop");
    }
    consume(TokenType::Semicolon, "Expected ; after continue");
    return std::make_unique<ContinueNode>();
}

std::unique_ptr<ASTNode> Parser::ifStatement() {
    auto node = std::make_unique<IfNode>();
    consume(TokenType::LParen, "Expected ( after if");
    node->condition = expression();
    consume(TokenType::RParen, "Expected )");
    if (check(TokenType::LBrace)) {
        node->thenBlock = block();
    } else {
        node->thenBlock.push_back(statement());
    }
    if (match({TokenType::Else})) {
        if (check(TokenType::LBrace)) {
            node->elseBlock = block();
        } else {
            node->elseBlock.push_back(statement());
        }
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::whileStatement() {
    auto node = std::make_unique<WhileNode>();
    consume(TokenType::LParen, "Expected ( after while");
    node->condition = expression();
    consume(TokenType::RParen, "Expected )");
    loopDepth++;
    node->body = block();
    loopDepth--;
    return node;
}

std::unique_ptr<ASTNode> Parser::forStatement() {
    auto node = std::make_unique<ForNode>();
    consume(TokenType::LParen, "Expected ( after for");

    if (!check(TokenType::Semicolon)) {
        if (check(TokenType::Int) || check(TokenType::Void) || check(TokenType::Float) ||
            check(TokenType::Double) || check(TokenType::Char)) {
            std::string type = advance().value;
            std::string name = consume(TokenType::Identifier, "Expected identifier").value;
            std::unique_ptr<ExprNode> initExpr = nullptr;
            if (match({TokenType::Assign})) {
                initExpr = expression();
            }
            node->init = std::make_unique<DeclarationNode>(type, name, false, false, std::move(initExpr));
            consume(TokenType::Semicolon, "Expected ; after for-init declaration");
        } else {
            node->init = expression();
            consume(TokenType::Semicolon, "Expected ; after for-init expression");
        }
    } else {
        consume(TokenType::Semicolon, "Expected ;");
    }

    if (!check(TokenType::Semicolon)) {
        node->condition = expression();
    }
    consume(TokenType::Semicolon, "Expected ; after for condition");

    if (!check(TokenType::RParen)) {
        node->increment = expression();
    }
    consume(TokenType::RParen, "Expected ) after for clauses");

    loopDepth++;
    node->body = block();
    loopDepth--;
    return node;
}

std::vector<std::unique_ptr<ASTNode>> Parser::block() {
    consume(TokenType::LBrace, "Expected {");
    std::vector<std::unique_ptr<ASTNode>> stmts;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        stmts.push_back(statement());
    }
    consume(TokenType::RBrace, "Expected }");
    return stmts;
}

std::unique_ptr<ExprNode> Parser::expression() {
    return assignment();
}

std::unique_ptr<ExprNode> Parser::assignment() {
    auto lhs = equality();
    if (match({TokenType::Assign})) {
        auto value = assignment();
        if (auto var = dynamic_cast<VariableNode*>(lhs.get())) {
            return std::make_unique<BinaryOpNode>("=", std::move(lhs), std::move(value));
        }
        throw std::runtime_error("Invalid assignment target");
    }
    return lhs;
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
    while (match({TokenType::Lt, TokenType::Leq, TokenType::Gt, TokenType::Geq})) {
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
    if (match({TokenType::Number})) {
        return std::make_unique<IntLiteralNode>(std::stoi(previous().value));
    }
    if (match({TokenType::Identifier})) {
        return std::make_unique<VariableNode>(previous().value);
    }
    if (match({TokenType::LParen})) {
        auto expr = expression();
        consume(TokenType::RParen, "Expected ) after expression");
        return expr;
    }
    throw std::runtime_error("Expected expression");
}