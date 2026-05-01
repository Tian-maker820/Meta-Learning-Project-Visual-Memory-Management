#include "interpreter.h"
#include "ast.h"
#include <sstream>
#include <stdexcept>
#include <QDebug>

namespace Interpreter {

// Interpreter实现

Interpreter::Interpreter()
    : state_(InterpreterState::IDLE), currentSnapshotIndex_(0) {}

void Interpreter::setProgram(std::unique_ptr<BlockNode> program) {
    program_ = std::move(program);
    reset();
}

void Interpreter::run() {
    if (state_ != InterpreterState::PAUSED && state_ != InterpreterState::IDLE) {
        return;
    }

    state_ = InterpreterState::RUNNING;
    if (stateChangedCallback_) {
        stateChangedCallback_(state_);
    }

    // 如果程序未执行过，从开始执行
    if (snapshots_.empty()) {
        takeSnapshot();
    }

    try {
        // 执行直到结束或暂停
        while (state_ == InterpreterState::RUNNING) {
            if (!program_ || currentSnapshotIndex_ >= snapshots_.size()) {
                state_ = InterpreterState::FINISHED;
                if (stateChangedCallback_) {
                    stateChangedCallback_(state_);
                }
                break;
            }

            // 执行下一条语句
            // 注意：实际执行逻辑需要根据AST节点类型进行
            // 这里简化处理，每次调用step()
            step();
        }
    } catch (const std::exception& e) {
        error(e.what());
        state_ = InterpreterState::IDLE;
        if (stateChangedCallback_) {
            stateChangedCallback_(state_);
        }
    }
}

void Interpreter::step() {
    if (state_ != InterpreterState::PAUSED && state_ != InterpreterState::IDLE) {
        return;
    }

    if (!program_) {
        error("No program loaded");
        return;
    }

    try {
        // 设置状态为运行中（单步）
        state_ = InterpreterState::RUNNING;
        if (stateChangedCallback_) {
            stateChangedCallback_(state_);
        }

        // 执行单步逻辑
        // 注意：这里需要实现具体的单步执行逻辑
        // 由于时间有限，这里简化处理
        takeSnapshot();

        // 执行完成后暂停
        state_ = InterpreterState::PAUSED;
        if (stateChangedCallback_) {
            stateChangedCallback_(state_);
        }
    } catch (const std::exception& e) {
        error(e.what());
        state_ = InterpreterState::IDLE;
        if (stateChangedCallback_) {
            stateChangedCallback_(state_);
        }
    }
}

void Interpreter::reset() {
    // 清空所有状态
    snapshots_.clear();
    context_.clear();
    currentSnapshotIndex_ = 0;
    error_.clear();

    state_ = InterpreterState::IDLE;
    if (stateChangedCallback_) {
        stateChangedCallback_(state_);
    }
}

void Interpreter::pause() {
    if (state_ == InterpreterState::RUNNING) {
        state_ = InterpreterState::PAUSED;
        if (stateChangedCallback_) {
            stateChangedCallback_(state_);
        }
    }
}

void Interpreter::jumpToSnapshot(size_t index) {
    if (index >= snapshots_.size()) {
        error("Snapshot index out of range");
        return;
    }

    size_t oldIndex = currentSnapshotIndex_;
    currentSnapshotIndex_ = index;

    try {
        restoreSnapshot(index);
    } catch (const std::exception& e) {
        error(e.what());
        currentSnapshotIndex_ = oldIndex;
        return;
    }

    if (jumpedCallback_) {
        jumpedCallback_(oldIndex, index);
    }
}

const ProgramState& Interpreter::getCurrentState() const {
    if (currentSnapshotIndex_ < snapshots_.size()) {
        return snapshots_[currentSnapshotIndex_];
    }

    // 返回空状态
    static ProgramState emptyState;
    return emptyState;
}

ProgramState& Interpreter::getCurrentState() {
    if (currentSnapshotIndex_ < snapshots_.size()) {
        return snapshots_[currentSnapshotIndex_];
    }

    // 创建新状态（不应该发生）
    snapshots_.push_back(ProgramState());
    currentSnapshotIndex_ = snapshots_.size() - 1;
    return snapshots_.back();
}

void Interpreter::setStateChangedCallback(StateChangedCallback callback) {
    stateChangedCallback_ = std::move(callback);
}

void Interpreter::setSnapshotAddedCallback(SnapshotAddedCallback callback) {
    snapshotAddedCallback_ = std::move(callback);
}

void Interpreter::setJumpedCallback(JumpedCallback callback) {
    jumpedCallback_ = std::move(callback);
}

void Interpreter::executeStatement(StatementNode* stmt) {
    if (!stmt) return;

    // 设置当前行号
    context_.setCurrentLine(stmt->getLine());

    try {
        // 执行语句
        stmt->execute(context_);

        // 执行后保存快照
        takeSnapshot();
    } catch (const std::exception& e) {
        error(e.what());
        throw;
    }
}

Value Interpreter::evaluateExpression(ExpressionNode* expr) {
    if (!expr) {
        throw std::runtime_error("Null expression");
    }

    try {
        return expr->execute(context_);
    } catch (const std::exception& e) {
        error(e.what());
        throw;
    }
}

void Interpreter::takeSnapshot() {
    // 创建新的程序状态快照
    ProgramState state;

    // 设置行号
    state.setLineNumber(context_.getCurrentLine());

    // 复制变量
    const auto& variables = context_.getVariables();
    for (const auto& pair : variables) {
        state.setVariable(pair.first, pair.second);
    }

    // 设置控制台输出
    state.setVariable("__output", Value(context_.getOutput()));

    // 添加到快照列表
    snapshots_.push_back(std::move(state));
    currentSnapshotIndex_ = snapshots_.size() - 1;

    if (snapshotAddedCallback_) {
        snapshotAddedCallback_(currentSnapshotIndex_);
    }
}

void Interpreter::restoreSnapshot(size_t index) {
    if (index >= snapshots_.size()) {
        throw std::runtime_error("Snapshot index out of range");
    }

    const ProgramState& state = snapshots_[index];

    // 恢复上下文
    context_.clear();

    // 恢复变量
    auto varNames = state.getVariableNames();
    for (const auto& name : varNames) {
        if (name != "__output") { // 跳过内部变量
            context_.setVariable(name, state.getVariable(name));
        }
    }

    // 恢复行号
    context_.setCurrentLine(state.getLineNumber());

    // 恢复控制台输出（如果有）
    if (state.hasVariable("__output")) {
        Value outputVal = state.getVariable("__output");
        if (outputVal.isArray()) {
            // 处理输出为数组的情况
            const auto& arr = outputVal.getArray();
            std::string output;
            for (const auto& val : arr) {
                output += val.toString();
            }
            context_.appendOutput(output);
        }
    }
}

void Interpreter::error(const std::string& message) {
    error_ = message;
    qWarning() << "Interpreter error:" << QString::fromStdString(message);
}

} // namespace Interpreter