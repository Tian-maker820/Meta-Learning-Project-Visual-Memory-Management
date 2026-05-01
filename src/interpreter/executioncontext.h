#ifndef EXECUTIONCONTEXT_H
#define EXECUTIONCONTEXT_H

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include "value.h"

namespace Interpreter {

// 执行上下文类
// 管理变量作用域、控制台输出和程序状态快照
class ExecutionContext {
public:
    // 构造函数
    ExecutionContext();

    // 变量管理
    void setVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;

    // 数组变量管理
    void setArrayElement(const std::string& name, size_t index, const Value& value);
    Value getArrayElement(const std::string& name, size_t index) const;
    size_t getArraySize(const std::string& name) const;

    // 控制台输出
    void appendOutput(const std::string& text);
    const std::string& getOutput() const { return output_; }
    void clearOutput();

    // 获取所有变量（用于状态快照）
    const std::unordered_map<std::string, Value>& getVariables() const {
        return variables_;
    }

    // 清空所有状态
    void clear();

    // 进入新的作用域（用于块语句）
    void enterScope();
    void exitScope();

    // 检查是否在当前作用域中
    bool isInCurrentScope(const std::string& name) const;

    // 获取当前执行行号（用于高亮显示）
    void setCurrentLine(int line) { currentLine_ = line; }
    int getCurrentLine() const { return currentLine_; }

private:
    // 变量存储（支持作用域）
    std::vector<std::unordered_map<std::string, Value>> scopes_;

    // 控制台输出缓冲区
    std::string output_;

    // 当前执行的行号
    int currentLine_ = 0;
};

} // namespace Interpreter

#endif // EXECUTIONCONTEXT_H