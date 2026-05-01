#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <string>
#include "value.h"
#include "tokentype.h"

namespace Interpreter {

// 前向声明，用于执行上下文
class ExecutionContext;

// AST节点基类
class ASTNode {
public:
    virtual ~ASTNode() = default;

    // 获取行号（用于调试和高亮）
    virtual int getLine() const = 0;

    // 执行节点，返回执行结果（对于表达式）或是否继续（对于语句）
    virtual Value execute(ExecutionContext& context) const = 0;

    // 获取节点类型的字符串表示
    virtual std::string typeString() const = 0;

    // 获取节点的字符串表示（用于调试）
    virtual std::string toString() const = 0;
};

// 表达式节点基类
class ExpressionNode : public ASTNode {
public:
    virtual Value evaluate(ExecutionContext& context) const {
        return execute(context);
    }
};

// 字面量表达式节点
class LiteralNode : public ExpressionNode {
public:
    LiteralNode(const Value& value, int line);
    int getLine() const override;
    Value execute(ExecutionContext& context) const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    Value value_;
    int line_;
};

// 变量访问节点
class VariableNode : public ExpressionNode {
public:
    VariableNode(const std::string& name, int line);
    int getLine() const override;
    Value execute(ExecutionContext& context) const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::string name_;
    int line_;
};

// 二元操作节点
class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(TokenType op, std::unique_ptr<ExpressionNode> left,
                 std::unique_ptr<ExpressionNode> right, int line);
    int getLine() const override;
    Value execute(ExecutionContext& context) const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    TokenType op_;
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
    int line_;
};

// 一元操作节点
class UnaryOpNode : public ExpressionNode {
public:
    UnaryOpNode(TokenType op, std::unique_ptr<ExpressionNode> expr, int line);
    int getLine() const override;
    Value execute(ExecutionContext& context) const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    TokenType op_;
    std::unique_ptr<ExpressionNode> expr_;
    int line_;
};

// 数组访问节点
class ArrayAccessNode : public ExpressionNode {
public:
    ArrayAccessNode(const std::string& name,
                    std::unique_ptr<ExpressionNode> index, int line);
    int getLine() const override;
    Value execute(ExecutionContext& context) const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::string name_;
    std::unique_ptr<ExpressionNode> index_;
    int line_;
};

// 语句节点基类
class StatementNode : public ASTNode {
public:
    // 语句执行不返回值
    Value execute(ExecutionContext& context) const override;
};

// 变量声明语句
class VariableDeclarationNode : public StatementNode {
public:
    VariableDeclarationNode(const std::string& name, const std::string& type,
                            std::unique_ptr<ExpressionNode> initializer,
                            int line);
    int getLine() const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::string name_;
    std::string type_;
    std::unique_ptr<ExpressionNode> initializer_;
    int line_;
};

// 赋值语句
class AssignmentNode : public StatementNode {
public:
    AssignmentNode(const std::string& name,
                   std::unique_ptr<ExpressionNode> value,
                   std::unique_ptr<ExpressionNode> index, // 用于数组赋值
                   int line);
    int getLine() const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::string name_;
    std::unique_ptr<ExpressionNode> value_;
    std::unique_ptr<ExpressionNode> index_; // nullptr表示普通变量赋值
    int line_;
};

// 打印语句
class PrintNode : public StatementNode {
public:
    PrintNode(std::unique_ptr<ExpressionNode> expr, int line);
    int getLine() const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::unique_ptr<ExpressionNode> expr_;
    int line_;
};

// If语句
class IfNode : public StatementNode {
public:
    IfNode(std::unique_ptr<ExpressionNode> condition,
           std::unique_ptr<StatementNode> thenBranch,
           std::unique_ptr<StatementNode> elseBranch, int line);
    int getLine() const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementNode> thenBranch_;
    std::unique_ptr<StatementNode> elseBranch_;
    int line_;
};

// While循环语句
class WhileNode : public StatementNode {
public:
    WhileNode(std::unique_ptr<ExpressionNode> condition,
              std::unique_ptr<StatementNode> body, int line);
    int getLine() const override;
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementNode> body_;
    int line_;
};

// 块语句（多个语句的序列）
class BlockNode : public StatementNode {
public:
    BlockNode(std::vector<std::unique_ptr<StatementNode>> statements, int line);
    int getLine() const override;
    void addStatement(std::unique_ptr<StatementNode> stmt);
    std::string typeString() const override;
    std::string toString() const override;

private:
    std::vector<std::unique_ptr<StatementNode>> statements_;
    int line_;
};

} // namespace Interpreter

#endif // AST_H