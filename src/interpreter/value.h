#ifndef VALUE_H
#define VALUE_H

#include <variant>
#include <string>
#include <vector>
#include <memory>

// 时间旅行调试器的值类型系统
// 使用std::variant实现多类型值，支持int、float、bool和数组

namespace Interpreter {

// 前向声明，用于数组存储
class Value;

// 数组类型定义
using Array = std::vector<Value>;

// 值类型：支持int、float、bool、数组
using ValueType = std::variant<
    int,        // 整数类型
    float,      // 浮点类型
    bool,       // 布尔类型
    Array       // 数组类型（递归定义）
>;

class Value {
public:
    // 默认构造函数，创建未初始化值
    Value();

    // 从各种类型构造
    Value(int val);
    Value(float val);
    Value(bool val);
    Value(const Array& val);

    // 获取值的字符串表示（用于调试和显示）
    std::string toString() const;

    // 获取值的类型字符串
    std::string typeString() const;

    // 检查是否为特定类型
    bool isInt() const;
    bool isFloat() const;
    bool isBool() const;
    bool isArray() const;

    // 获取值（需要确保类型正确）
    int getInt() const;
    float getFloat() const;
    bool getBool() const;
    const Array& getArray() const;

    // 设置值
    void setInt(int val);
    void setFloat(float val);
    void setBool(bool val);
    void setArray(const Array& val);

    // 数组操作
    size_t arraySize() const;
    const Value& arrayAt(size_t index) const;
    Value& arrayAt(size_t index);
    void arrayPush(const Value& val);
    void arraySet(size_t index, const Value& val);

    // 比较操作（用于if和while条件）
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;

    // 算术操作（四则运算）
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;

    // 逻辑操作
    Value operator&&(const Value& other) const;
    Value operator||(const Value& other) const;
    Value operator!() const;

private:
    ValueType value_;
};

} // namespace Interpreter

#endif // VALUE_H