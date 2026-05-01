#include "ast.h"
#include "executioncontext.h"
#include <sstream>
#include <stdexcept>

namespace Interpreter {

// LiteralNode实现
LiteralNode::LiteralNode(const Value& value, int line)
    : value_(value), line_(line) {}

int LiteralNode::getLine() const { return line_; }

Value LiteralNode::execute(ExecutionContext& context) const {
    return value_;
}

std::string LiteralNode::typeString() const {
    return "Literal";
}

std::string LiteralNode::toString() const {
    return "Literal(" + value_.toString() + ")";
}

// VariableNode实现
VariableNode::VariableNode(const std::string& name, int line)
    : name_(name), line_(line) {}

int VariableNode::getLine() const { return line_; }

Value VariableNode::execute(ExecutionContext& context) const {
    if (!context.hasVariable(name_)) {
        throw std::runtime_error("Undefined variable: " + name_);
    }
    return context.getVariable(name_);
}

std::string VariableNode::typeString() const {
    return "Variable";
}

std::string VariableNode::toString() const {
    return "Variable(" + name_ + ")";
}

// BinaryOpNode实现
BinaryOpNode::BinaryOpNode(TokenType op,
                           std::unique_ptr<ExpressionNode> left,
                           std::unique_ptr<ExpressionNode> right,
                           int line)
    : op_(op), left_(std::move(left)), right_(std::move(right)), line_(line) {}

int BinaryOpNode::getLine() const { return line_; }

Value BinaryOpNode::execute(ExecutionContext& context) const {
    Value leftVal = left_->execute(context);
    Value rightVal = right_->execute(context);

    switch (op_) {
        case TokenType::OP_PLUS:
            return leftVal + rightVal;
        case TokenType::OP_MINUS:
            return leftVal - rightVal;
        case TokenType::OP_MULTIPLY:
            return leftVal * rightVal;
        case TokenType::OP_DIVIDE:
            return leftVal / rightVal;
        case TokenType::OP_EQUAL:
            return Value(leftVal == rightVal);
        case TokenType::OP_NOT_EQUAL:
            return Value(leftVal != rightVal);
        case TokenType::OP_LESS:
            return Value(leftVal < rightVal);
        case TokenType::OP_LESS_EQUAL:
            return Value(leftVal <= rightVal);
        case TokenType::OP_GREATER:
            return Value(leftVal > rightVal);
        case TokenType::OP_GREATER_EQUAL:
            return Value(leftVal >= rightVal);
        case TokenType::OP_AND:
            return leftVal && rightVal;
        case TokenType::OP_OR:
            return leftVal || rightVal;
        default:
            throw std::runtime_error("Unsupported binary operator");
    }
}

std::string BinaryOpNode::typeString() const {
    return "BinaryOp";
}

std::string BinaryOpNode::toString() const {
    std::string opStr;
    switch (op_) {
        case TokenType::OP_PLUS: opStr = "+"; break;
        case TokenType::OP_MINUS: opStr = "-"; break;
        case TokenType::OP_MULTIPLY: opStr = "*"; break;
        case TokenType::OP_DIVIDE: opStr = "/"; break;
        case TokenType::OP_EQUAL: opStr = "=="; break;
        case TokenType::OP_NOT_EQUAL: opStr = "!="; break;
        case TokenType::OP_LESS: opStr = "<"; break;
        case TokenType::OP_LESS_EQUAL: opStr = "<="; break;
        case TokenType::OP_GREATER: opStr = ">"; break;
        case TokenType::OP_GREATER_EQUAL: opStr = ">="; break;
        case TokenType::OP_AND: opStr = "&&"; break;
        case TokenType::OP_OR: opStr = "||"; break;
        default: opStr = "?";
    }
    return "BinaryOp(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
}

// UnaryOpNode实现
UnaryOpNode::UnaryOpNode(TokenType op,
                         std::unique_ptr<ExpressionNode> expr,
                         int line)
    : op_(op), expr_(std::move(expr)), line_(line) {}

int UnaryOpNode::getLine() const { return line_; }

Value UnaryOpNode::execute(ExecutionContext& context) const {
    Value val = expr_->execute(context);

    switch (op_) {
        case TokenType::OP_NOT:
            return !val;
        case TokenType::OP_MINUS:  // 负号
            if (val.isInt()) {
                return Value(-val.getInt());
            } else if (val.isFloat()) {
                return Value(-val.getFloat());
            } else {
                throw std::runtime_error("Cannot apply unary minus to non-numeric value");
            }
        default:
            throw std::runtime_error("Unsupported unary operator");
    }
}

std::string UnaryOpNode::typeString() const {
    return "UnaryOp";
}

std::string UnaryOpNode::toString() const {
    std::string opStr;
    switch (op_) {
        case TokenType::OP_NOT: opStr = "!"; break;
        case TokenType::OP_MINUS: opStr = "-"; break;
        default: opStr = "?";
    }
    return "UnaryOp(" + opStr + expr_->toString() + ")";
}

// ArrayAccessNode实现
ArrayAccessNode::ArrayAccessNode(const std::string& name,
                                 std::unique_ptr<ExpressionNode> index,
                                 int line)
    : name_(name), index_(std::move(index)), line_(line) {}

int ArrayAccessNode::getLine() const { return line_; }

Value ArrayAccessNode::execute(ExecutionContext& context) const {
    if (!context.hasVariable(name_)) {
        throw std::runtime_error("Undefined array: " + name_);
    }

    Value indexVal = index_->execute(context);
    if (!indexVal.isInt()) {
        throw std::runtime_error("Array index must be integer");
    }

    int idx = indexVal.getInt();
    if (idx < 0) {
        throw std::runtime_error("Array index cannot be negative");
    }

    return context.getArrayElement(name_, static_cast<size_t>(idx));
}

std::string ArrayAccessNode::typeString() const {
    return "ArrayAccess";
}

std::string ArrayAccessNode::toString() const {
    return "ArrayAccess(" + name_ + "[" + index_->toString() + "])";
}

// StatementNode基类的execute实现
Value StatementNode::execute(ExecutionContext& context) const {
    // 语句执行不返回值
    return Value(0);
}

// VariableDeclarationNode实现
VariableDeclarationNode::VariableDeclarationNode(const std::string& name,
                                                 const std::string& type,
                                                 std::unique_ptr<ExpressionNode> initializer,
                                                 int line)
    : name_(name), type_(type), initializer_(std::move(initializer)), line_(line) {}

int VariableDeclarationNode::getLine() const { return line_; }

std::string VariableDeclarationNode::typeString() const {
    return "VariableDeclaration";
}

std::string VariableDeclarationNode::toString() const {
    std::string initStr = initializer_ ? " = " + initializer_->toString() : "";
    return "VarDecl(" + type_ + " " + name_ + initStr + ")";
}

// AssignmentNode实现
AssignmentNode::AssignmentNode(const std::string& name,
                               std::unique_ptr<ExpressionNode> value,
                               std::unique_ptr<ExpressionNode> index,
                               int line)
    : name_(name), value_(std::move(value)), index_(std::move(index)), line_(line) {}

int AssignmentNode::getLine() const { return line_; }

std::string AssignmentNode::typeString() const {
    return "Assignment";
}

std::string AssignmentNode::toString() const {
    std::string indexStr = index_ ? "[" + index_->toString() + "]" : "";
    return "Assign(" + name_ + indexStr + " = " + value_->toString() + ")";
}

// PrintNode实现
PrintNode::PrintNode(std::unique_ptr<ExpressionNode> expr, int line)
    : expr_(std::move(expr)), line_(line) {}

int PrintNode::getLine() const { return line_; }

std::string PrintNode::typeString() const {
    return "Print";
}

std::string PrintNode::toString() const {
    return "Print(" + expr_->toString() + ")";
}

// IfNode实现
IfNode::IfNode(std::unique_ptr<ExpressionNode> condition,
               std::unique_ptr<StatementNode> thenBranch,
               std::unique_ptr<StatementNode> elseBranch,
               int line)
    : condition_(std::move(condition)),
      thenBranch_(std::move(thenBranch)),
      elseBranch_(std::move(elseBranch)),
      line_(line) {}

int IfNode::getLine() const { return line_; }

std::string IfNode::typeString() const {
    return "If";
}

std::string IfNode::toString() const {
    std::string elseStr = elseBranch_ ? " else " + elseBranch_->toString() : "";
    return "If(" + condition_->toString() + ") " + thenBranch_->toString() + elseStr;
}

// WhileNode实现
WhileNode::WhileNode(std::unique_ptr<ExpressionNode> condition,
                     std::unique_ptr<StatementNode> body,
                     int line)
    : condition_(std::move(condition)), body_(std::move(body)), line_(line) {}

int WhileNode::getLine() const { return line_; }

std::string WhileNode::typeString() const {
    return "While";
}

std::string WhileNode::toString() const {
    return "While(" + condition_->toString() + ") " + body_->toString();
}

// BlockNode实现
BlockNode::BlockNode(std::vector<std::unique_ptr<StatementNode>> statements, int line)
    : statements_(std::move(statements)), line_(line) {}

int BlockNode::getLine() const { return line_; }

void BlockNode::addStatement(std::unique_ptr<StatementNode> stmt) {
    statements_.push_back(std::move(stmt));
}

std::string BlockNode::typeString() const {
    return "Block";
}

std::string BlockNode::toString() const {
    std::string result = "Block{\n";
    for (const auto& stmt : statements_) {
        result += "  " + stmt->toString() + "\n";
    }
    result += "}";
    return result;
}

} // namespace Interpreter