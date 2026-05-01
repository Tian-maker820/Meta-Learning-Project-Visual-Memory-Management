#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "tokenizer.h"
#include "ast.h"

namespace Interpreter {

// 解析错误异常
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, int line, int column)
        : std::runtime_error(message + " at line " + std::to_string(line) +
                            ", column " + std::to_string(column)),
          line_(line), column_(column) {}

    int getLine() const { return line_; }
    int getColumn() const { return column_; }

private:
    int line_;
    int column_;
};

// 递归下降解析器类
// 实现简单C语言子集的语法分析
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // 解析整个程序，返回AST根节点
    std::unique_ptr<BlockNode> parseProgram();

    // 获取解析错误信息
    std::string getError() const { return error_; }
    bool hasError() const { return !error_.empty(); }

private:
    // 解析方法（按优先级从低到高）
    std::unique_ptr<StatementNode> parseStatement();
    std::unique_ptr<StatementNode> parseVariableDeclaration();
    std::unique_ptr<StatementNode> parseAssignment();
    std::unique_ptr<StatementNode> parsePrintStatement();
    std::unique_ptr<StatementNode> parseIfStatement();
    std::unique_ptr<StatementNode> parseWhileStatement();
    std::unique_ptr<StatementNode> parseBlock();

    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<ExpressionNode> parseLogicalOr();
    std::unique_ptr<ExpressionNode> parseLogicalAnd();
    std::unique_ptr<ExpressionNode> parseEquality();
    std::unique_ptr<ExpressionNode> parseComparison();
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parseUnary();
    std::unique_ptr<ExpressionNode> parsePrimary();

    // 数组相关解析
    std::unique_ptr<ExpressionNode> parseArrayAccess();
    std::unique_ptr<ExpressionNode> parseArrayLiteral();

    // 辅助方法
    Token consume(TokenType type, const std::string& errorMessage);
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token advance();
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;

    // 错误处理
    ParseError error(const Token& token, const std::string& message);
    void synchronize();

private:
    std::vector<Token> tokens_;
    std::string error_;
    int current_ = 0;
};

} // namespace Interpreter

#endif // PARSER_H