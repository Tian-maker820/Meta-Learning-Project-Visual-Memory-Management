#include "tokentype.h"
#include <unordered_map>
#include <cctype>

namespace Interpreter {

std::string tokenTypeString(TokenType type) {
    static const std::unordered_map<TokenType, std::string> typeStrings = {
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::KW_INT, "KW_INT"},
        {TokenType::KW_FLOAT, "KW_FLOAT"},
        {TokenType::KW_BOOL, "KW_BOOL"},
        {TokenType::KW_IF, "KW_IF"},
        {TokenType::KW_ELSE, "KW_ELSE"},
        {TokenType::KW_WHILE, "KW_WHILE"},
        {TokenType::KW_PRINT, "KW_PRINT"},
        {TokenType::KW_TRUE, "KW_TRUE"},
        {TokenType::KW_FALSE, "KW_FALSE"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::OP_PLUS, "OP_PLUS"},
        {TokenType::OP_MINUS, "OP_MINUS"},
        {TokenType::OP_MULTIPLY, "OP_MULTIPLY"},
        {TokenType::OP_DIVIDE, "OP_DIVIDE"},
        {TokenType::OP_ASSIGN, "OP_ASSIGN"},
        {TokenType::OP_EQUAL, "OP_EQUAL"},
        {TokenType::OP_NOT_EQUAL, "OP_NOT_EQUAL"},
        {TokenType::OP_LESS, "OP_LESS"},
        {TokenType::OP_LESS_EQUAL, "OP_LESS_EQUAL"},
        {TokenType::OP_GREATER, "OP_GREATER"},
        {TokenType::OP_GREATER_EQUAL, "OP_GREATER_EQUAL"},
        {TokenType::OP_AND, "OP_AND"},
        {TokenType::OP_OR, "OP_OR"},
        {TokenType::OP_NOT, "OP_NOT"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    };

    auto it = typeStrings.find(type);
    if (it != typeStrings.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

std::string Token::typeString() const {
    return tokenTypeString(type);
}

std::string Token::toString() const {
    return "Token[" + typeString() + ", \"" + text + "\", line=" +
           std::to_string(line) + ", col=" + std::to_string(column) + "]";
}

bool Token::isOneOf(std::initializer_list<TokenType> types) const {
    for (TokenType t : types) {
        if (type == t) {
            return true;
        }
    }
    return false;
}

bool isKeyword(const std::string& str) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"int", TokenType::KW_INT},
        {"float", TokenType::KW_FLOAT},
        {"bool", TokenType::KW_BOOL},
        {"if", TokenType::KW_IF},
        {"else", TokenType::KW_ELSE},
        {"while", TokenType::KW_WHILE},
        {"print", TokenType::KW_PRINT},
        {"true", TokenType::KW_TRUE},
        {"false", TokenType::KW_FALSE},
    };

    // 将字符串转换为小写进行比较（C语言关键字是大小写敏感的，但我们简化处理）
    std::string lowerStr;
    lowerStr.reserve(str.size());
    for (char c : str) {
        lowerStr.push_back(std::tolower(static_cast<unsigned char>(c)));
    }

    return keywords.find(lowerStr) != keywords.end();
}

TokenType keywordToTokenType(const std::string& str) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"int", TokenType::KW_INT},
        {"float", TokenType::KW_FLOAT},
        {"bool", TokenType::KW_BOOL},
        {"if", TokenType::KW_IF},
        {"else", TokenType::KW_ELSE},
        {"while", TokenType::KW_WHILE},
        {"print", TokenType::KW_PRINT},
        {"true", TokenType::KW_TRUE},
        {"false", TokenType::KW_FALSE},
    };

    std::string lowerStr;
    lowerStr.reserve(str.size());
    for (char c : str) {
        lowerStr.push_back(std::tolower(static_cast<unsigned char>(c)));
    }

    auto it = keywords.find(lowerStr);
    if (it != keywords.end()) {
        return it->second;
    }

    // 如果不是关键字，返回END_OF_FILE（不应该发生）
    return TokenType::END_OF_FILE;
}

} // namespace Interpreter