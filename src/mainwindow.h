#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include <memory>
#include "interpreter/interpreter.h"

// 前向声明Qt部件
class EditorWidget;
class VariableWidget;
class ConsoleWidget;
class ControlWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // 运行控制
    void onRunClicked();
    void onStepClicked();
    void onResetClicked();
    void onPauseClicked();

    // 时间旅行
    void onSliderValueChanged(int value);
    void onSliderPressed();
    void onSliderReleased();

    // 代码编辑
    void onCodeChanged();

    // 解释器状态变化
    void onInterpreterStateChanged(Interpreter::InterpreterState state);
    void onSnapshotAdded(size_t index);
    void onJumped(size_t from, size_t to);

private:
    // 初始化方法
    void setupUI();
    void setupConnections();
    void loadDefaultCode();

    // 工具方法
    void updateUIFromState();
    void highlightCurrentLine(int line);
    void clearHighlights();

    // 运行控制
    void startExecution();
    void stopExecution();

    // 成员变量
    std::unique_ptr<Interpreter::Interpreter> interpreter_;

    // UI部件
    EditorWidget* editorWidget_ = nullptr;
    VariableWidget* variableWidget_ = nullptr;
    ConsoleWidget* consoleWidget_ = nullptr;
    ControlWidget* controlWidget_ = nullptr;
    QSlider* timelineSlider_ = nullptr;
    QPushButton* runButton_ = nullptr;
    QPushButton* stepButton_ = nullptr;
    QPushButton* resetButton_ = nullptr;
    QPushButton* pauseButton_ = nullptr;

    // 执行状态
    bool isExecuting_ = false;
    bool isTimeTraveling_ = false;
    QTimer* executionTimer_ = nullptr;
};

#endif // MAINWINDOW_H