#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QRegExp>

// 简单语法高亮器，支持C语言子集
class SimpleHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SimpleHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules_;

    QTextCharFormat keywordFormat_;
    QTextCharFormat typeFormat_;
    QTextCharFormat numberFormat_;
    QTextCharFormat stringFormat_;
    QTextCharFormat commentFormat_;
};

// 代码编辑器部件
class EditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit EditorWidget(QWidget* parent = nullptr);

    // 获取和设置代码
    QString getCode() const;
    void setCode(const QString& code);

    // 行号显示
    void setLineNumbersVisible(bool visible);
    bool isLineNumbersVisible() const;

    // 高亮特定行（用于调试）
    void highlightLine(int lineNumber, const QColor& color = Qt::yellow);
    void clearHighlights();

    // 获取当前行号（光标所在行）
    int getCurrentLine() const;

    // 设置只读模式
    void setReadOnly(bool readOnly);

signals:
    void codeChanged();

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);
    void highlightCurrentLine();

private:
    // 内部类：行号区域
    class LineNumberArea : public QWidget {
    public:
        explicit LineNumberArea(EditorWidget* editor) : QWidget(editor), editor_(editor) {}

        QSize sizeHint() const override {
            return QSize(editor_->lineNumberAreaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent* event) override {
            editor_->lineNumberAreaPaintEvent(event);
        }

    private:
        EditorWidget* editor_;
    };

    // 初始化方法
    void setupUI();
    void setupHighlighter();

    // 行号区域相关
    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent* event);

    // 成员变量
    QPlainTextEdit* textEdit_ = nullptr;
    LineNumberArea* lineNumberArea_ = nullptr;
    SimpleHighlighter* highlighter_ = nullptr;
    bool showLineNumbers_ = true;

    // 高亮行存储
    QMap<int, QColor> highlightedLines_;
};

#endif // EDITORWIDGET_H