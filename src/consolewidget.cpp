#include "consolewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QDebug>

ConsoleWidget::ConsoleWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void ConsoleWidget::setupUI() {
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建文本编辑区
    textEdit_ = new QPlainTextEdit(this);
    textEdit_->setReadOnly(true);
    textEdit_->setFont(QFont("Monospace", 10));
    textEdit_->setMaximumBlockCount(1000); // 限制最大行数，防止内存过大

    // 创建控制按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(5, 5, 5, 5);

    clearButton_ = new QPushButton("清空", this);
    clearButton_->setFixedWidth(80);

    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton_);

    // 添加到主布局
    mainLayout->addWidget(textEdit_, 1); // 文本编辑区占据大部分空间
    mainLayout->addLayout(buttonLayout, 0); // 按钮区域固定高度

    // 连接信号
    connect(clearButton_, &QPushButton::clicked,
            this, &ConsoleWidget::onClearClicked);
    connect(textEdit_, &QPlainTextEdit::textChanged,
            this, &ConsoleWidget::outputChanged);
}

void ConsoleWidget::appendOutput(const QString& text) {
    if (text.isEmpty()) return;

    textEdit_->appendPlainText(text);

    // 自动滚动到底部
    QTextCursor cursor = textEdit_->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit_->setTextCursor(cursor);
}

void ConsoleWidget::appendError(const QString& text) {
    if (text.isEmpty()) return;

    // 保存当前文本颜色
    QTextCursor cursor = textEdit_->textCursor();
    QTextCharFormat originalFormat = cursor.charFormat();

    // 设置错误颜色（红色）
    QTextCharFormat errorFormat;
    errorFormat.setForeground(Qt::red);

    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(errorFormat);
    cursor.insertText(text + "\n");

    // 恢复原格式
    cursor.setCharFormat(originalFormat);

    // 自动滚动到底部
    cursor.movePosition(QTextCursor::End);
    textEdit_->setTextCursor(cursor);
}

QString ConsoleWidget::getOutput() const {
    return textEdit_->toPlainText();
}

void ConsoleWidget::setOutput(const QString& text) {
    textEdit_->setPlainText(text);
}

void ConsoleWidget::clear() {
    textEdit_->clear();
}

void ConsoleWidget::setReadOnly(bool readOnly) {
    textEdit_->setReadOnly(readOnly);
}

void ConsoleWidget::onClearClicked() {
    clear();
}