#include "parser.h"
#include <sstream>
#include <stdexcept>
#include <memory>

namespace Interpreter {

// Parser实现

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0) {}

std::unique_ptr<BlockNode> Parser::parseProgram() {
    try {
        std::vector<std::unique_ptr<StatementNode>> statements;

        while (!isAtEnd()) {
            statements.push_back(parseStatement());
        }

        return std::make_unique<BlockNode>(std::move(statements), 0);
    } catch (const ParseError& e) {
        error_ = e.what();
        return nullptr;
    }
}

std::unique_ptr<StatementNode> Parser::parseStatement() {
    if (check(TokenType::KW_INT) || check(TokenType::KW_FLOAT) || check(TokenType::KW_BOOL)) {
        return parseVariableDeclaration();
    } else if (check(TokenType::KW_IF)) {
        return parseIfStatement();
    } else if (check(TokenType::KW_WHILE)) {
        return parseWhileStatement();
    } else if (check(TokenType::KW_PRINT)) {
        return parsePrintStatement();
    } else if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    } else if (check(TokenType::LEFT_BRACE)) {
        return parseBlock();
    } else {
        throw error(peek(), "Expected statement");
    }
}

std::unique_ptr<StatementNode> Parser::parseVariableDeclaration() {
    Token typeToken = advance(); // 类型关键字
    std::string typeName = typeToken.text;

    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
    std::string varName = nameToken.text;

    std::unique_ptr<ExpressionNode> initializer = nullptr;

    // 检查是否有初始化表达式
    if (match(TokenType::OP_ASSIGN)) {
        initializer = parseExpression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

    return std::make_unique<VariableDeclarationNode>(varName, typeName,
                                                    std::move(initializer),
                                                    typeToken.line);
}

std::unique_ptr<StatementNode> Parser::parseAssignment() {
    Token nameToken = advance(); // 变量名
    std::string varName = nameToken.text;

    std::unique_ptr<ExpressionNode> indexExpr = nullptr;

    // 检查是否为数组赋值
    if (match(TokenType::LEFT_BRACKET)) {
        indexExpr = parseExpression();
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after array index");
    }

    consume(TokenType::OP_ASSIGN, "Expected '=' after variable name");

    std::unique_ptr<ExpressionNode> valueExpr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");

    return std::make_unique<AssignmentNode>(varName, std::move(valueExpr),
                                           std::move(indexExpr), nameToken.line);
}

std::unique_ptr<StatementNode> Parser::parsePrintStatement() {
    Token printToken = consume(TokenType::KW_PRINT, "Expected 'print'");

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'print'");
    std::unique_ptr<ExpressionNode> expr = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
    consume(TokenType::SEMICOLON, "Expected ';' after print statement");

    return std::make_unique<PrintNode>(std::move(expr), printToken.line);
}

std::unique_ptr<StatementNode> Parser::parseIfStatement() {
    Token ifToken = consume(TokenType::KW_IF, "Expected 'if'");

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    std::unique_ptr<ExpressionNode> condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");

    std::unique_ptr<StatementNode> thenBranch = parseStatement();
    std::unique_ptr<StatementNode> elseBranch = nullptr;

    if (match(TokenType::KW_ELSE)) {
        elseBranch = parseStatement();
    }

    return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch),
                                   std::move(elseBranch), ifToken.line);
}

std::unique_ptr<StatementNode> Parser::parseWhileStatement() {
    Token whileToken = consume(TokenType::KW_WHILE, "Expected 'while'");

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    std::unique_ptr<ExpressionNode> condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");

    std::unique_ptr<StatementNode> body = parseStatement();

    return std::make_unique<WhileNode>(std::move(condition), std::move(body),
                                      whileToken.line);
}

std::unique_ptr<StatementNode> Parser::parseBlock() {
    Token braceToken = consume(TokenType::LEFT_BRACE, "Expected '{'");

    std::vector<std::unique_ptr<StatementNode>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after block");

    return std::make_unique<BlockNode>(std::move(statements), braceToken.line);
}

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    return parseLogicalOr();
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalOr() {
    std::unique_ptr<ExpressionNode> expr = parseLogicalAnd();

    while (match(TokenType::OP_OR)) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalAnd() {
    std::unique_ptr<ExpressionNode> expr = parseEquality();

    while (match(TokenType::OP_AND)) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseEquality() {
    std::unique_ptr<ExpressionNode> expr = parseComparison();

    while (match({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseComparison() {
    std::unique_ptr<ExpressionNode> expr = parseTerm();

    while (match({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL,
                 TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseTerm() {
    std::unique_ptr<ExpressionNode> expr = parseFactor();

    while (match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor() {
    std::unique_ptr<ExpressionNode> expr = parseUnary();

    while (match({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE})) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseUnary();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr),
                                             std::move(right), op.line);
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary() {
    if (match({TokenType::OP_NOT, TokenType::OP_MINUS})) {
        Token op = previous();
        std::unique_ptr<ExpressionNode> expr = parseUnary();
        return std::make_unique<UnaryOpNode>(op.type, std::move(expr), op.line);
    }

    return parsePrimary();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match(TokenType::KW_TRUE)) {
        return std::make_unique<LiteralNode>(Value(true), previous().line);
    }

    if (match(TokenType::KW_FALSE)) {
        return std::make_unique<LiteralNode>(Value(false), previous().line);
    }

    if (match(TokenType::INTEGER_LITERAL)) {
        int value = std::stoi(previous().text);
        return std::make_unique<LiteralNode>(Value(value), previous().line);
    }

    if (match(TokenType::FLOAT_LITERAL)) {
        float value = std::stof(previous().text);
        return std::make_unique<LiteralNode>(Value(value), previous().line);
    }

    if (match(TokenType::STRING_LITERAL)) {
        // 字符串字面量暂时不支持，返回空字符串
        return std::make_unique<LiteralNode>(Value(""), previous().line);
    }

    if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().text;
        int line = previous().line;

        // 检查是否为数组访问
        if (match(TokenType::LEFT_BRACKET)) {
            std::unique_ptr<ExpressionNode> index = parseExpression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after array index");
            return std::make_unique<ArrayAccessNode>(name, std::move(index), line);
        }

        return std::make_unique<VariableNode>(name, line);
    }

    if (match(TokenType::LEFT_PAREN)) {
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }

    throw error(peek(), "Expected expression");
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    throw error(peek(), errorMessage);
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) {
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

Token Parser::peek() const {
    if (isAtEnd()) return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens_[current_];
}

Token Parser::previous() const {
    if (current_ == 0) return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens_[current_ - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

ParseError Parser::error(const Token& token, const std::string& message) {
    return ParseError(message, token.line, token.column);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::KW_INT:
            case TokenType::KW_FLOAT:
            case TokenType::KW_BOOL:
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_PRINT:
                return;
            default:
                break;
        }

        advance();
    }
}

} // namespace Interpreter