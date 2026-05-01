#include "value.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace Interpreter {

// Value类实现

Value::Value() : value_(0) {}  // 默认值为int 0

Value::Value(int val) : value_(val) {}
Value::Value(float val) : value_(val) {}
Value::Value(bool val) : value_(val) {}
Value::Value(const Array& val) : value_(val) {}

std::string Value::toString() const {
    if (isInt()) {
        return std::to_string(getInt());
    } else if (isFloat()) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << getFloat();
        // 移除末尾多余的0
        std::string s = oss.str();
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
    } else if (isBool()) {
        return getBool() ? "true" : "false";
    } else if (isArray()) {
        const auto& arr = getArray();
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) result += ", ";
            result += arr[i].toString();
        }
        result += "]";
        return result;
    }
    return "undefined";
}

std::string Value::typeString() const {
    if (isInt()) return "int";
    if (isFloat()) return "float";
    if (isBool()) return "bool";
    if (isArray()) return "array";
    return "undefined";
}

bool Value::isInt() const { return std::holds_alternative<int>(value_); }
bool Value::isFloat() const { return std::holds_alternative<float>(value_); }
bool Value::isBool() const { return std::holds_alternative<bool>(value_); }
bool Value::isArray() const { return std::holds_alternative<Array>(value_); }

int Value::getInt() const {
    try {
        return std::get<int>(value_);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Value is not an integer");
    }
}

float Value::getFloat() const {
    try {
        return std::get<float>(value_);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Value is not a float");
    }
}

bool Value::getBool() const {
    try {
        return std::get<bool>(value_);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Value is not a boolean");
    }
}

const Array& Value::getArray() const {
    try {
        return std::get<Array>(value_);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Value is not an array");
    }
}

void Value::setInt(int val) { value_ = val; }
void Value::setFloat(float val) { value_ = val; }
void Value::setBool(bool val) { value_ = val; }
void Value::setArray(const Array& val) { value_ = val; }

size_t Value::arraySize() const {
    if (!isArray()) return 0;
    return getArray().size();
}

const Value& Value::arrayAt(size_t index) const {
    if (!isArray()) {
        throw std::runtime_error("Not an array");
    }
    const auto& arr = getArray();
    if (index >= arr.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return arr[index];
}

Value& Value::arrayAt(size_t index) {
    if (!isArray()) {
        throw std::runtime_error("Not an array");
    }
    auto& arr = std::get<Array>(value_);
    if (index >= arr.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return arr[index];
}

void Value::arrayPush(const Value& val) {
    if (!isArray()) {
        throw std::runtime_error("Not an array");
    }
    std::get<Array>(value_).push_back(val);
}

void Value::arraySet(size_t index, const Value& val) {
    if (!isArray()) {
        throw std::runtime_error("Not an array");
    }
    auto& arr = std::get<Array>(value_);
    if (index >= arr.size()) {
        arr.resize(index + 1);
    }
    arr[index] = val;
}

bool Value::operator==(const Value& other) const {
    if (isInt() && other.isInt()) {
        return getInt() == other.getInt();
    } else if (isFloat() && other.isFloat()) {
        return getFloat() == other.getFloat();
    } else if (isBool() && other.isBool()) {
        return getBool() == other.getBool();
    } else if (isArray() && other.isArray()) {
        const auto& arr1 = getArray();
        const auto& arr2 = other.getArray();
        if (arr1.size() != arr2.size()) return false;
        for (size_t i = 0; i < arr1.size(); ++i) {
            if (arr1[i] != arr2[i]) return false;
        }
        return true;
    }
    return false;
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

bool Value::operator<(const Value& other) const {
    if (isInt() && other.isInt()) {
        return getInt() < other.getInt();
    } else if (isFloat() && other.isFloat()) {
        return getFloat() < other.getFloat();
    } else if (isBool() && other.isBool()) {
        return getBool() < other.getBool();
    }
    throw std::runtime_error("Cannot compare values of these types with <");
}

bool Value::operator<=(const Value& other) const {
    return *this < other || *this == other;
}

bool Value::operator>(const Value& other) const {
    return !(*this <= other);
}

bool Value::operator>=(const Value& other) const {
    return !(*this < other);
}

Value Value::operator+(const Value& other) const {
    if (isInt() && other.isInt()) {
        return Value(getInt() + other.getInt());
    } else if (isFloat() && other.isFloat()) {
        return Value(getFloat() + other.getFloat());
    } else if (isInt() && other.isFloat()) {
        return Value(static_cast<float>(getInt()) + other.getFloat());
    } else if (isFloat() && other.isInt()) {
        return Value(getFloat() + static_cast<float>(other.getInt()));
    }
    throw std::runtime_error("Cannot add values of these types");
}

Value Value::operator-(const Value& other) const {
    if (isInt() && other.isInt()) {
        return Value(getInt() - other.getInt());
    } else if (isFloat() && other.isFloat()) {
        return Value(getFloat() - other.getFloat());
    } else if (isInt() && other.isFloat()) {
        return Value(static_cast<float>(getInt()) - other.getFloat());
    } else if (isFloat() && other.isInt()) {
        return Value(getFloat() - static_cast<float>(other.getInt()));
    }
    throw std::runtime_error("Cannot subtract values of these types");
}

Value Value::operator*(const Value& other) const {
    if (isInt() && other.isInt()) {
        return Value(getInt() * other.getInt());
    } else if (isFloat() && other.isFloat()) {
        return Value(getFloat() * other.getFloat());
    } else if (isInt() && other.isFloat()) {
        return Value(static_cast<float>(getInt()) * other.getFloat());
    } else if (isFloat() && other.isInt()) {
        return Value(getFloat() * static_cast<float>(other.getInt()));
    }
    throw std::runtime_error("Cannot multiply values of these types");
}

Value Value::operator/(const Value& other) const {
    if (isInt() && other.isInt()) {
        int divisor = other.getInt();
        if (divisor == 0) {
            throw std::runtime_error("Division by zero");
        }
        return Value(getInt() / divisor);
    } else if (isFloat() && other.isFloat()) {
        float divisor = other.getFloat();
        if (divisor == 0.0f) {
            throw std::runtime_error("Division by zero");
        }
        return Value(getFloat() / divisor);
    } else if (isInt() && other.isFloat()) {
        float divisor = other.getFloat();
        if (divisor == 0.0f) {
            throw std::runtime_error("Division by zero");
        }
        return Value(static_cast<float>(getInt()) / divisor);
    } else if (isFloat() && other.isInt()) {
        float divisor = static_cast<float>(other.getInt());
        if (divisor == 0.0f) {
            throw std::runtime_error("Division by zero");
        }
        return Value(getFloat() / divisor);
    }
    throw std::runtime_error("Cannot divide values of these types");
}

Value Value::operator&&(const Value& other) const {
    if (isBool() && other.isBool()) {
        return Value(getBool() && other.getBool());
    }
    throw std::runtime_error("Cannot apply logical AND to non-boolean values");
}

Value Value::operator||(const Value& other) const {
    if (isBool() && other.isBool()) {
        return Value(getBool() || other.getBool());
    }
    throw std::runtime_error("Cannot apply logical OR to non-boolean values");
}

Value Value::operator!() const {
    if (isBool()) {
        return Value(!getBool());
    }
    throw std::runtime_error("Cannot apply logical NOT to non-boolean value");
}

} // namespace Interpreter