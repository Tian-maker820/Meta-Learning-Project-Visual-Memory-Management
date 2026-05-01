#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include "tokentype.h"

namespace Interpreter {

// 词法分析器类，负责将源代码转换为Token序列
class Tokenizer {
public:
    // 构造函数，传入源代码字符串
    explicit Tokenizer(const std::string& source);

    // 获取所有Token
    std::vector<Token> tokenize();

    // 获取最后一个错误信息（如果有）
    std::string getError() const { return error_; }

    // 检查是否有错误
    bool hasError() const { return !error_.empty(); }

private:
    // 私有辅助方法
    void scanTokens();
    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& text);

    // 扫描特定类型的Token
    void scanIdentifierOrKeyword();
    void scanNumber();
    void scanString();

    // 工具方法
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    bool isAtEnd() const;

    // 错误处理
    void error(const std::string& message);

private:
    std::string source_;        // 源代码
    std::vector<Token> tokens_; // 生成的Token列表
    std::string error_;         // 错误信息

    // 扫描状态
    int start_ = 0;    // 当前Token的起始位置
    int current_ = 0;  // 当前扫描位置
    int line_ = 1;     // 当前行号（从1开始）
    int column_ = 1;   // 当前列号（从1开始）
};

} // namespace Interpreter

#endif // TOKENIZER_H