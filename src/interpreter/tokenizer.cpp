#include "tokenizer.h"
#include <cctype>
#include <sstream>

namespace Interpreter {

// Tokenizer实现

Tokenizer::Tokenizer(const std::string& source)
    : source_(source), start_(0), current_(0), line_(1), column_(1) {}

std::vector<Token> Tokenizer::tokenize() {
    tokens_.clear();
    error_.clear();

    start_ = 0;
    current_ = 0;
    line_ = 1;
    column_ = 1;

    try {
        while (!isAtEnd()) {
            start_ = current_;
            scanToken();
        }

        // 添加文件结束Token
        addToken(TokenType::END_OF_FILE, "");
    } catch (const std::runtime_error& e) {
        error_ = e.what();
    }

    return tokens_;
}

void Tokenizer::scanToken() {
    char c = advance();

    switch (c) {
        // 单字符Token
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case ';': addToken(TokenType::SEMICOLON); break;

        // 可能的多字符Token
        case '=':
            addToken(match('=') ? TokenType::OP_EQUAL : TokenType::OP_ASSIGN);
            break;
        case '!':
            addToken(match('=') ? TokenType::OP_NOT_EQUAL : TokenType::OP_NOT);
            break;
        case '<':
            addToken(match('=') ? TokenType::OP_LESS_EQUAL : TokenType::OP_LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::OP_GREATER_EQUAL : TokenType::OP_GREATER);
            break;
        case '&':
            if (match('&')) {
                addToken(TokenType::OP_AND);
            } else {
                error("Expected '&' after '&'");
            }
            break;
        case '|':
            if (match('|')) {
                addToken(TokenType::OP_OR);
            } else {
                error("Expected '|' after '|'");
            }
            break;
        case '+': addToken(TokenType::OP_PLUS); break;
        case '-': addToken(TokenType::OP_MINUS); break;
        case '*': addToken(TokenType::OP_MULTIPLY); break;
        case '/':
            if (match('/')) {
                // 单行注释，跳过直到行尾
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // 多行注释
                while (!isAtEnd()) {
                    if (peek() == '*' && peekNext() == '/') {
                        advance(); // 跳过 '*'
                        advance(); // 跳过 '/'
                        break;
                    }
                    advance();
                }
                if (isAtEnd()) {
                    error("Unterminated multi-line comment");
                }
            } else {
                addToken(TokenType::OP_DIVIDE);
            }
            break;

        // 空白字符
        case ' ':
        case '\t':
        case '\r':
            // 忽略
            break;
        case '\n':
            line_++;
            column_ = 1;
            break;

        // 字符串字面量
        case '"': scanString(); break;

        default:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                scanNumber();
            } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                scanIdentifierOrKeyword();
            } else {
                error("Unexpected character: " + std::string(1, c));
            }
            break;
    }
}

void Tokenizer::scanIdentifierOrKeyword() {
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    if (isKeyword(text)) {
        addToken(keywordToTokenType(text), text);
    } else {
        addToken(TokenType::IDENTIFIER, text);
    }
}

void Tokenizer::scanNumber() {
    bool hasDecimal = false;

    while (std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    // 检查小数部分
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext()))) {
        hasDecimal = true;
        advance(); // 跳过 '.'

        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
        }
    }

    std::string text = source_.substr(start_, current_ - start_);
    TokenType type = hasDecimal ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    addToken(type, text);
}

void Tokenizer::scanString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        error("Unterminated string");
        return;
    }

    // 跳过结束的引号
    advance();

    // 提取字符串内容（不包括引号）
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void Tokenizer::addToken(TokenType type) {
    std::string text = source_.substr(start_, current_ - start_);
    addToken(type, text);
}

void Tokenizer::addToken(TokenType type, const std::string& text) {
    Token token(type, text, line_, column_);
    tokens_.push_back(token);
    column_ += static_cast<int>(text.length());
}

char Tokenizer::advance() {
    if (isAtEnd()) return '\0';
    current_++;
    return source_[current_ - 1];
}

char Tokenizer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Tokenizer::peekNext() const {
    if (current_ + 1 >= source_.size()) return '\0';
    return source_[current_ + 1];
}

bool Tokenizer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;

    current_++;
    column_++;
    return true;
}

bool Tokenizer::isAtEnd() const {
    return current_ >= source_.size();
}

void Tokenizer::error(const std::string& message) {
    throw std::runtime_error(message + " at line " + std::to_string(line_) +
                            ", column " + std::to_string(column_));
}

} // namespace Interpreter