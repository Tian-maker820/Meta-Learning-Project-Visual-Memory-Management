#include "programstate.h"
#include <sstream>

namespace Interpreter {

// ProgramState实现

ProgramState::ProgramState() : lineNumber_(0) {}

ProgramState::ProgramState(int lineNumber, const std::string& consoleOutput)
    : lineNumber_(lineNumber), consoleOutput_(consoleOutput) {}

ProgramState::ProgramState(const ProgramState& other) {
    copyVariablesFrom(other);
    lineNumber_ = other.lineNumber_;
    consoleOutput_ = other.consoleOutput_;
}

ProgramState::ProgramState(ProgramState&& other) noexcept
    : variables_(std::move(other.variables_)),
      lineNumber_(other.lineNumber_),
      consoleOutput_(std::move(other.consoleOutput_)) {
    other.lineNumber_ = 0;
}

ProgramState& ProgramState::operator=(const ProgramState& other) {
    if (this != &other) {
        copyVariablesFrom(other);
        lineNumber_ = other.lineNumber_;
        consoleOutput_ = other.consoleOutput_;
    }
    return *this;
}

ProgramState& ProgramState::operator=(ProgramState&& other) noexcept {
    if (this != &other) {
        variables_ = std::move(other.variables_);
        lineNumber_ = other.lineNumber_;
        consoleOutput_ = std::move(other.consoleOutput_);
        other.lineNumber_ = 0;
    }
    return *this;
}

void ProgramState::setLineNumber(int line) {
    lineNumber_ = line;
}

void ProgramState::appendConsoleOutput(const std::string& text) {
    consoleOutput_ += text;
}

void ProgramState::setVariable(const std::string& name, const Value& value) {
    variables_[name] = value;
}

Value ProgramState::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw std::runtime_error("Variable not found: " + name);
    }
    return it->second;
}

bool ProgramState::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::vector<std::string> ProgramState::getVariableNames() const {
    std::vector<std::string> names;
    names.reserve(variables_.size());
    for (const auto& pair : variables_) {
        names.push_back(pair.first);
    }
    return names;
}

void ProgramState::clear() {
    variables_.clear();
    lineNumber_ = 0;
    consoleOutput_.clear();
}

void ProgramState::mergeConsoleOutput(const ProgramState& previousState) {
    // 只保留相对于前一个状态新增的输出
    if (consoleOutput_.size() >= previousState.consoleOutput_.size()) {
        consoleOutput_ = consoleOutput_.substr(previousState.consoleOutput_.size());
    } else {
        // 这种情况不应该发生，但为了安全起见
        consoleOutput_.clear();
    }
}

std::string ProgramState::toString() const {
    std::ostringstream oss;
    oss << "ProgramState {\n";
    oss << "  lineNumber: " << lineNumber_ << "\n";
    oss << "  variables: " << variables_.size() << "\n";
    for (const auto& pair : variables_) {
        oss << "    " << pair.first << " = " << pair.second.toString();
        oss << " (" << pair.second.typeString() << ")\n";
    }
    oss << "  consoleOutput: \"" << consoleOutput_ << "\"\n";
    oss << "}";
    return oss.str();
}

void ProgramState::copyVariablesFrom(const ProgramState& other) {
    variables_.clear();
    for (const auto& pair : other.variables_) {
        variables_[pair.first] = pair.second;
    }
}

} // namespace Interpreter