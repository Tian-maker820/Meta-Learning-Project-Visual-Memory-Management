#include "editorwidget.h"
#include <QPainter>
#include <QTextBlock>
#include <QFont>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>

// SimpleHighlighter实现
SimpleHighlighter::SimpleHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    // 关键字格式（蓝色粗体）
    keywordFormat_.setForeground(Qt::darkBlue);
    keywordFormat_.setFontWeight(QFont::Bold);

    // 类型格式（深绿色）
    typeFormat_.setForeground(Qt::darkGreen);
    typeFormat_.setFontWeight(QFont::Bold);

    // 数字格式（深红色）
    numberFormat_.setForeground(Qt::darkRed);

    // 字符串格式（暗青色）
    stringFormat_.setForeground(Qt::darkCyan);

    // 注释格式（灰色）
    commentFormat_.setForeground(Qt::gray);

    // 定义关键字
    QStringList keywordPatterns;
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
                    << "\\bprint\\b" << "\\btrue\\b" << "\\bfalse\\b";

    foreach (const QString &pattern, keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat_;
        highlightingRules_.append(rule);
    }

    // 类型关键字
    QStringList typePatterns;
    typePatterns << "\\bint\\b" << "\\bfloat\\b" << "\\bbool\\b";

    foreach (const QString &pattern, typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegExp(pattern);
        rule.format = typeFormat_;
        highlightingRules_.append(rule);
    }

    // 数字
    HighlightingRule numberRule;
    numberRule.pattern = QRegExp("\\b[0-9]+(\\.[0-9]+)?\\b");
    numberRule.format = numberFormat_;
    highlightingRules_.append(numberRule);

    // 字符串
    HighlightingRule stringRule;
    stringRule.pattern = QRegExp("\"[^\"]*\"");
    stringRule.format = stringFormat_;
    highlightingRules_.append(stringRule);

    // 单行注释
    HighlightingRule commentRule;
    commentRule.pattern = QRegExp("//[^\n]*");
    commentRule.format = commentFormat_;
    highlightingRules_.append(commentRule);
}

void SimpleHighlighter::highlightBlock(const QString& text) {
    foreach (const HighlightingRule &rule, highlightingRules_) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}

// EditorWidget实现
EditorWidget::EditorWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    setupHighlighter();
}

void EditorWidget::setupUI() {
    // 创建主布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建文本编辑器
    textEdit_ = new QPlainTextEdit(this);
    textEdit_->setFont(QFont("Monospace", 10));
    textEdit_->setTabStopDistance(40); // 设置tab宽度为4个字符

    // 创建行号区域
    lineNumberArea_ = new LineNumberArea(this);

    // 设置布局
    layout->addWidget(textEdit_);

    // 连接信号
    connect(textEdit_->document(), &QTextDocument::contentsChanged,
            this, &EditorWidget::codeChanged);
    connect(textEdit_->verticalScrollBar(), &QScrollBar::valueChanged,
            this, [this]() { lineNumberArea_->update(); });
    connect(textEdit_, &QPlainTextEdit::textChanged,
            this, &EditorWidget::updateLineNumberAreaWidth);
    connect(textEdit_, &QPlainTextEdit::cursorPositionChanged,
            this, &EditorWidget::highlightCurrentLine);
    connect(textEdit_->document(), &QTextDocument::blockCountChanged,
            this, &EditorWidget::updateLineNumberAreaWidth);

    // 初始更新
    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

void EditorWidget::setupHighlighter() {
    highlighter_ = new SimpleHighlighter(textEdit_->document());
}

QString EditorWidget::getCode() const {
    return textEdit_->toPlainText();
}

void EditorWidget::setCode(const QString& code) {
    textEdit_->setPlainText(code);
    clearHighlights();
}

void EditorWidget::setLineNumbersVisible(bool visible) {
    showLineNumbers_ = visible;
    lineNumberArea_->setVisible(visible);
    updateLineNumberAreaWidth();
}

bool EditorWidget::isLineNumbersVisible() const {
    return showLineNumbers_;
}

void EditorWidget::highlightLine(int lineNumber, const QColor& color) {
    if (lineNumber < 1) return;

    // 将行号转换为0-based索引
    QTextBlock block = textEdit_->document()->findBlockByLineNumber(lineNumber - 1);
    if (block.isValid()) {
        highlightedLines_[lineNumber] = color;

        // 创建一个选择来高亮该行
        QList<QTextEdit::ExtraSelection> extraSelections = textEdit_->extraSelections();
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(color);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = QTextCursor(block);
        selection.cursor.clearSelection();

        extraSelections.append(selection);
        textEdit_->setExtraSelections(extraSelections);
    }
}

void EditorWidget::clearHighlights() {
    highlightedLines_.clear();
    textEdit_->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    highlightCurrentLine(); // 重新高亮当前行
}

int EditorWidget::getCurrentLine() const {
    QTextCursor cursor = textEdit_->textCursor();
    return cursor.blockNumber() + 1; // 转换为1-based行号
}

void EditorWidget::setReadOnly(bool readOnly) {
    textEdit_->setReadOnly(readOnly);
    highlightCurrentLine(); // 更新高亮
}

int EditorWidget::lineNumberAreaWidth() const {
    if (!showLineNumbers_) return 0;

    int digits = 1;
    int max = qMax(1, textEdit_->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 计算宽度：数字宽度 + 边距
    QFontMetrics metrics(textEdit_->font());
    int space = 3 + metrics.horizontalAdvance(QLatin1Char('9')) * digits + 3;
    return space;
}

void EditorWidget::updateLineNumberAreaWidth() {
    if (showLineNumbers_) {
        textEdit_->setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    } else {
        textEdit_->setViewportMargins(0, 0, 0, 0);
    }
}

void EditorWidget::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy) {
        lineNumberArea_->scroll(0, dy);
    } else {
        lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());
    }
}

void EditorWidget::highlightCurrentLine() {
    // 清除之前的高亮，但保留手动高亮的行
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 添加手动高亮的行
    for (auto it = highlightedLines_.constBegin(); it != highlightedLines_.constEnd(); ++it) {
        int lineNumber = it.key();
        QColor color = it.value();

        QTextBlock block = textEdit_->document()->findBlockByLineNumber(lineNumber - 1);
        if (block.isValid()) {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(color);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            QTextCursor cursor(block);
            cursor.clearSelection();
            selection.cursor = cursor;
            extraSelections.append(selection);
        }
    }

    // 添加当前行高亮（浅灰色）
    if (textEdit_->isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(240, 240, 240);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textEdit_->textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }

    textEdit_->setExtraSelections(extraSelections);
}

void EditorWidget::lineNumberAreaPaintEvent(QPaintEvent* event) {
    if (!showLineNumbers_) return;

    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = textEdit_->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)textEdit_->blockBoundingGeometry(block).translated(textEdit_->contentOffset()).top();
    int bottom = top + (int)textEdit_->blockBoundingRect(block).height();

    // 绘制行号
    QFont font = textEdit_->font();
    QFontMetrics metrics(font);
    painter.setFont(font);
    painter.setPen(Qt::darkGray);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea_->width() - 3,
                            metrics.height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)textEdit_->blockBoundingRect(block).height();
        ++blockNumber;
    }
}