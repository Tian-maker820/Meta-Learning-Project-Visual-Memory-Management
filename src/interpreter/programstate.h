#ifndef PROGRAMSTATE_H
#define PROGRAMSTATE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "value.h"

namespace Interpreter {

// 程序状态快照类
// 用于存储程序在某个时刻的完整状态，包括所有变量值和执行位置
// 时间旅行调试器的核心数据结构
class ProgramState {
public:
    // 构造函数
    ProgramState();

    // 带参数的构造函数
    ProgramState(int lineNumber, const std::string& consoleOutput);

    // 拷贝构造函数（深拷贝）
    ProgramState(const ProgramState& other);

    // 移动构造函数
    ProgramState(ProgramState&& other) noexcept;

    // 拷贝赋值运算符（深拷贝）
    ProgramState& operator=(const ProgramState& other);

    // 移动赋值运算符
    ProgramState& operator=(ProgramState&& other) noexcept;

    // 设置行号（当前执行的代码行）
    void setLineNumber(int line);
    int getLineNumber() const { return lineNumber_; }

    // 添加控制台输出
    void appendConsoleOutput(const std::string& text);
    const std::string& getConsoleOutput() const { return consoleOutput_; }

    // 变量管理
    void setVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;

    // 获取所有变量名（用于UI显示）
    std::vector<std::string> getVariableNames() const;

    // 获取变量数量
    size_t variableCount() const { return variables_.size(); }

    // 清空所有状态
    void clear();

    // 合并控制台输出（用于增量更新）
    void mergeConsoleOutput(const ProgramState& previousState);

    // 获取状态的字符串表示（用于调试）
    std::string toString() const;

private:
    // 内部变量存储
    std::unordered_map<std::string, Value> variables_;

    // 执行位置信息
    int lineNumber_;           // 当前执行的行号

    // 控制台输出
    std::string consoleOutput_; // 到当前步骤为止的所有控制台输出

    // 辅助方法：深拷贝变量表
    void copyVariablesFrom(const ProgramState& other);
};

} // namespace Interpreter

#endif // PROGRAMSTATE_H