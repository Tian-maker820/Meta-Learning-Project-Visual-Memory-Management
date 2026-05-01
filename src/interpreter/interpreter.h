#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <memory>
#include <functional>
#include "ast.h"
#include "programstate.h"
#include "executioncontext.h"

namespace Interpreter {

// 解释器状态枚举
enum class InterpreterState {
    IDLE,      // 空闲状态
    RUNNING,   // 正在执行
    PAUSED,    // 暂停状态（单步执行）
    FINISHED   // 执行完成
};

// 解释器类
// 负责执行AST并管理程序状态快照
class Interpreter {
public:
    // 构造函数
    Interpreter();

    // 设置程序AST
    void setProgram(std::unique_ptr<BlockNode> program);

    // 执行控制
    void run();          // 运行到结束
    void step();         // 单步执行
    void reset();        // 重置到初始状态
    void pause();        // 暂停执行

    // 时间旅行功能
    void jumpToSnapshot(size_t index);  // 跳转到指定快照
    size_t getCurrentSnapshotIndex() const { return currentSnapshotIndex_; }
    size_t getSnapshotCount() const { return snapshots_.size(); }

    // 获取当前状态
    const ProgramState& getCurrentState() const;
    ProgramState& getCurrentState();
    InterpreterState getState() const { return state_; }

    // 状态变化回调
    using StateChangedCallback = std::function<void(InterpreterState)>;
    using SnapshotAddedCallback = std::function<void(size_t index)>;
    using JumpedCallback = std::function<void(size_t from, size_t to)>;

    void setStateChangedCallback(StateChangedCallback callback);
    void setSnapshotAddedCallback(SnapshotAddedCallback callback);
    void setJumpedCallback(JumpedCallback callback);

    // 错误处理
    std::string getError() const { return error_; }
    bool hasError() const { return !error_.empty(); }
    void clearError() { error_.clear(); }

private:
    // 执行辅助方法
    void executeStatement(StatementNode* stmt);
    Value evaluateExpression(ExpressionNode* expr);

    // 快照管理
    void takeSnapshot();
    void restoreSnapshot(size_t index);

    // 错误处理
    void error(const std::string& message);

    // 执行状态变量
    InterpreterState state_ = InterpreterState::IDLE;
    std::unique_ptr<BlockNode> program_;
    ExecutionContext context_;

    // 快照存储
    std::vector<ProgramState> snapshots_;
    size_t currentSnapshotIndex_ = 0;

    // 回调函数
    StateChangedCallback stateChangedCallback_;
    SnapshotAddedCallback snapshotAddedCallback_;
    JumpedCallback jumpedCallback_;

    // 错误信息
    std::string error_;
};

} // namespace Interpreter

#endif // INTERPRETER_H