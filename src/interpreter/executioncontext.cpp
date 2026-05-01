#include "executioncontext.h"
#include <stdexcept>

namespace Interpreter {

ExecutionContext::ExecutionContext() {
    // 创建全局作用域
    scopes_.push_back(std::unordered_map<std::string, Value>());
}

void ExecutionContext::setVariable(const std::string& name, const Value& value) {
    if (scopes_.empty()) {
        throw std::runtime_error("No active scope");
    }

    // 设置变量在当前作用域
    scopes_.back()[name] = value;
}

Value ExecutionContext::getVariable(const std::string& name) const {
    // 从内到外搜索作用域
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            return varIt->second;
        }
    }

    throw std::runtime_error("Undefined variable: " + name);
}

bool ExecutionContext::hasVariable(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

void ExecutionContext::setArrayElement(const std::string& name, size_t index, const Value& value) {
    Value arrayVal = getVariable(name);
    if (!arrayVal.isArray()) {
        throw std::runtime_error("Variable is not an array: " + name);
    }

    // 获取数组引用并设置元素
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            varIt->second.arraySet(index, value);
            return;
        }
    }

    throw std::runtime_error("Undefined array: " + name);
}

Value ExecutionContext::getArrayElement(const std::string& name, size_t index) const {
    Value arrayVal = getVariable(name);
    if (!arrayVal.isArray()) {
        throw std::runtime_error("Variable is not an array: " + name);
    }
    return arrayVal.arrayAt(index);
}

size_t ExecutionContext::getArraySize(const std::string& name) const {
    Value arrayVal = getVariable(name);
    if (!arrayVal.isArray()) {
        throw std::runtime_error("Variable is not an array: " + name);
    }
    return arrayVal.arraySize();
}

void ExecutionContext::appendOutput(const std::string& text) {
    output_ += text;
}

void ExecutionContext::clearOutput() {
    output_.clear();
}

void ExecutionContext::clear() {
    scopes_.clear();
    scopes_.push_back(std::unordered_map<std::string, Value>());
    output_.clear();
    currentLine_ = 0;
}

void ExecutionContext::enterScope() {
    scopes_.push_back(std::unordered_map<std::string, Value>());
}

void ExecutionContext::exitScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

bool ExecutionContext::isInCurrentScope(const std::string& name) const {
    if (scopes_.empty()) return false;
    return scopes_.back().find(name) != scopes_.back().end();
}

} // namespace Interpreter