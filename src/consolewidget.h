#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class ConsoleWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget* parent = nullptr);

    // 添加输出
    void appendOutput(const QString& text);
    void appendError(const QString& text);

    // 获取和设置输出内容
    QString getOutput() const;
    void setOutput(const QString& text);

    // 清空控制台
    void clear();

    // 获取文本编辑部件（用于布局等）
    QPlainTextEdit* getTextEdit() const { return textEdit_; }

    // 设置只读模式
    void setReadOnly(bool readOnly);

signals:
    void outputChanged();

private slots:
    void onClearClicked();

private:
    // 初始化UI
    void setupUI();

    // 成员变量
    QPlainTextEdit* textEdit_ = nullptr;
    QPushButton* clearButton_ = nullptr;
};

#endif // CONSOLEWIDGET_H