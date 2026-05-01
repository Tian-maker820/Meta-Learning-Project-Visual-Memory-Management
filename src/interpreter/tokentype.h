#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <string>

namespace Interpreter {

// Token类型枚举，支持简单C语言子集
enum class TokenType {
    // 文件结束
    END_OF_FILE,

    // 关键字
    KW_INT,      // int
    KW_FLOAT,    // float
    KW_BOOL,     // bool
    KW_IF,       // if
    KW_ELSE,     // else
    KW_WHILE,    // while
    KW_PRINT,    // print
    KW_TRUE,     // true
    KW_FALSE,    // false

    // 标识符
    IDENTIFIER,

    // 字面量
    INTEGER_LITERAL,    // 整数，如42
    FLOAT_LITERAL,      // 浮点数，如3.14
    STRING_LITERAL,     // 字符串，如"hello"

    // 运算符
    OP_PLUS,            // +
    OP_MINUS,           // -
    OP_MULTIPLY,        // *
    OP_DIVIDE,          // /
    OP_ASSIGN,          // =
    OP_EQUAL,           // ==
    OP_NOT_EQUAL,       // !=
    OP_LESS,            // <
    OP_LESS_EQUAL,      // <=
    OP_GREATER,         // >
    OP_GREATER_EQUAL,   // >=
    OP_AND,             // &&
    OP_OR,              // ||
    OP_NOT,             // !

    // 分隔符
    SEMICOLON,          // ;
    COMMA,              // ,
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
};

// Token结构体，包含类型、文本内容、位置信息
struct Token {
    TokenType type;
    std::string text;
    int line;   // 行号（从1开始）
    int column; // 列号（从1开始）

    // 构造函数
    Token(TokenType type = TokenType::END_OF_FILE,
          const std::string& text = "",
          int line = 0, int column = 0)
        : type(type), text(text), line(line), column(column) {}

    // 获取Token类型的字符串表示
    std::string typeString() const;

    // 获取完整的调试信息
    std::string toString() const;

    // 检查是否为特定类型
    bool is(TokenType t) const { return type == t; }
    bool isOneOf(std::initializer_list<TokenType> types) const;
};

// Token类型的字符串表示
std::string tokenTypeString(TokenType type);

// 判断是否为关键字
bool isKeyword(const std::string& str);

// 将关键字字符串转换为TokenType
TokenType keywordToTokenType(const std::string& str);

} // namespace Interpreter

#endif // TOKENTYPE_H