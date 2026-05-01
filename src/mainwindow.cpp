#include "mainwindow.h"
#include "editorwidget.h"
#include "variablewidget.h"
#include "consolewidget.h"
#include "controlwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>

#include "interpreter/tokenizer.h"
#include "interpreter/parser.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    // 创建解释器
    interpreter_ = std::make_unique<Interpreter::Interpreter>();

    // 设置窗口属性
    setWindowTitle("时间旅行调试器 - 课程大作业");
    resize(1200, 800);

    // 初始化UI
    setupUI();
    setupConnections();
    loadDefaultCode();

    // 创建执行定时器
    executionTimer_ = new QTimer(this);
    executionTimer_->setInterval(100); // 100ms间隔
    connect(executionTimer_, &QTimer::timeout, this, &MainWindow::onRunClicked);
}

MainWindow::~MainWindow() {
    // 停止执行
    stopExecution();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // 停止执行
    stopExecution();

    // 确认关闭
    if (isExecuting_) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认关闭",
                                      "程序正在执行，确定要关闭吗？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();
}

void MainWindow::setupUI() {
    // 创建中央部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 创建水平分割器（左侧编辑器，右侧变量和控制台）
    QSplitter* horizontalSplitter = new QSplitter(Qt::Horizontal, centralWidget);

    // 左侧：编辑器
    editorWidget_ = new EditorWidget(horizontalSplitter);
    horizontalSplitter->addWidget(editorWidget_);

    // 右侧：创建垂直分割器（上部分变量，下部分控制台）
    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, horizontalSplitter);

    // 右上：变量监控
    variableWidget_ = new VariableWidget(verticalSplitter);
    verticalSplitter->addWidget(variableWidget_);

    // 右下：控制台输出
    consoleWidget_ = new ConsoleWidget(verticalSplitter);
    verticalSplitter->addWidget(consoleWidget_);

    // 设置垂直分割器的初始比例
    verticalSplitter->setSizes({300, 200});

    // 将垂直分割器添加到水平分割器
    horizontalSplitter->addWidget(verticalSplitter);

    // 设置水平分割器的初始比例
    horizontalSplitter->setSizes({600, 400});

    // 底部：控制面板
    controlWidget_ = new ControlWidget(centralWidget);
    timelineSlider_ = controlWidget_->getTimelineSlider();
    runButton_ = controlWidget_->getRunButton();
    stepButton_ = controlWidget_->getStepButton();
    resetButton_ = controlWidget_->getResetButton();
    pauseButton_ = controlWidget_->getPauseButton();

    // 添加到主布局
    mainLayout->addWidget(horizontalSplitter, 1); // 占据大部分空间
    mainLayout->addWidget(controlWidget_, 0);     // 固定高度

    // 设置编辑器为只读（执行时）
    editorWidget_->setReadOnly(false);
}

void MainWindow::setupConnections() {
    // 解释器回调
    interpreter_->setStateChangedCallback([this](Interpreter::InterpreterState state) {
        QMetaObject::invokeMethod(this, [this, state]() {
            onInterpreterStateChanged(state);
        });
    });

    interpreter_->setSnapshotAddedCallback([this](size_t index) {
        QMetaObject::invokeMethod(this, [this, index]() {
            onSnapshotAdded(index);
        });
    });

    interpreter_->setJumpedCallback([this](size_t from, size_t to) {
        QMetaObject::invokeMethod(this, [this, from, to]() {
            onJumped(from, to);
        });
    });

    // 控制部件信号
    connect(controlWidget_, &ControlWidget::runClicked,
            this, &MainWindow::onRunClicked);
    connect(controlWidget_, &ControlWidget::stepClicked,
            this, &MainWindow::onStepClicked);
    connect(controlWidget_, &ControlWidget::resetClicked,
            this, &MainWindow::onResetClicked);
    connect(controlWidget_, &ControlWidget::pauseClicked,
            this, &MainWindow::onPauseClicked);
    connect(controlWidget_, &ControlWidget::sliderValueChanged,
            this, &MainWindow::onSliderValueChanged);
    connect(controlWidget_, &ControlWidget::sliderPressed,
            this, &MainWindow::onSliderPressed);
    connect(controlWidget_, &ControlWidget::sliderReleased,
            this, &MainWindow::onSliderReleased);

    // 编辑器信号
    connect(editorWidget_, &EditorWidget::codeChanged,
            this, &MainWindow::onCodeChanged);
}

void MainWindow::loadDefaultCode() {
    // 加载示例代码
    QString defaultCode = R"(
// 时间旅行调试器示例程序
// 支持 int, float, bool 类型和简单数组

int main() {
    // 变量声明和初始化
    int x = 10;
    float y = 3.14;
    bool flag = true;

    // 数组声明
    int arr[5];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = x;

    // 算术运算
    int sum = x + 5;
    float product = y * 2.0;

    // 条件语句
    if (x > 5) {
        print("x is greater than 5");
    } else {
        print("x is 5 or less");
    }

    // 循环语句
    int i = 0;
    while (i < 3) {
        print("Loop iteration: ");
        print(i);
        i = i + 1;
    }

    // 数组访问
    print("First array element: ");
    print(arr[0]);

    return 0;
}
)";

    editorWidget_->setCode(defaultCode);
}

void MainWindow::onRunClicked() {
    if (!isExecuting_) {
        startExecution();
    }
}

void MainWindow::onStepClicked() {
    if (!isExecuting_ && !isTimeTraveling_) {
        // 解析并执行代码
        QString code = editorWidget_->getCode();
        if (code.isEmpty()) return;

        try {
            // 词法分析
            Interpreter::Tokenizer tokenizer(code.toStdString());
            auto tokens = tokenizer.tokenize();
            if (tokenizer.hasError()) {
                consoleWidget_->appendError(QString::fromStdString(tokenizer.getError()));
                return;
            }

            // 语法分析
            Interpreter::Parser parser(tokens);
            auto program = parser.parseProgram();
            if (parser.hasError()) {
                consoleWidget_->appendError(QString::fromStdString(parser.getError()));
                return;
            }

            // 设置程序并执行单步
            interpreter_->setProgram(std::move(program));
            interpreter_->step();

            // 更新UI
            updateUIFromState();
        } catch (const std::exception& e) {
            consoleWidget_->appendError(QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::onResetClicked() {
    // 停止执行
    stopExecution();

    // 重置解释器
    interpreter_->reset();

    // 重置UI
    consoleWidget_->clear();
    variableWidget_->clearVariables();
    editorWidget_->clearHighlights();
    controlWidget_->setStepCount(0, 0);
    controlWidget_->setStatusText("就绪");

    // 重置时间轴
    timelineSlider_->setValue(0);
    timelineSlider_->setEnabled(false);
}

void MainWindow::onPauseClicked() {
    if (isExecuting_) {
        stopExecution();
        interpreter_->pause();
    }
}

void MainWindow::onSliderValueChanged(int value) {
    if (isTimeTraveling_) {
        // 跳转到指定快照
        size_t snapshotIndex = static_cast<size_t>(value);
        interpreter_->jumpToSnapshot(snapshotIndex);

        // 更新UI
        updateUIFromState();
    }
}

void MainWindow::onSliderPressed() {
    isTimeTraveling_ = true;
}

void MainWindow::onSliderReleased() {
    isTimeTraveling_ = false;
}

void MainWindow::onCodeChanged() {
    // 代码改变时重置状态
    if (!isExecuting_) {
        onResetClicked();
    }
}

void MainWindow::onInterpreterStateChanged(Interpreter::InterpreterState state) {
    switch (state) {
        case Interpreter::InterpreterState::IDLE:
            controlWidget_->setStatusText("就绪");
            controlWidget_->setRunningState(false);
            controlWidget_->setPausedState(false);
            controlWidget_->setFinishedState(false);
            break;
        case Interpreter::InterpreterState::RUNNING:
            controlWidget_->setStatusText("运行中");
            controlWidget_->setRunningState(true);
            break;
        case Interpreter::InterpreterState::PAUSED:
            controlWidget_->setStatusText("已暂停");
            controlWidget_->setPausedState(true);
            break;
        case Interpreter::InterpreterState::FINISHED:
            controlWidget_->setStatusText("已完成");
            controlWidget_->setFinishedState(true);
            stopExecution();
            break;
    }
}

void MainWindow::onSnapshotAdded(size_t index) {
    size_t total = interpreter_->getSnapshotCount();
    controlWidget_->setStepCount(static_cast<int>(index + 1), static_cast<int>(total));

    // 更新时间轴滑块
    timelineSlider_->setRange(0, static_cast<int>(total));
    timelineSlider_->setValue(static_cast<int>(index));
    timelineSlider_->setEnabled(total > 0);
}

void MainWindow::onJumped(size_t from, size_t to) {
    Q_UNUSED(from);
    controlWidget_->setStepCount(static_cast<int>(to + 1),
                                static_cast<int>(interpreter_->getSnapshotCount()));
}

void MainWindow::updateUIFromState() {
    const auto& state = interpreter_->getCurrentState();

    // 更新变量显示
    auto varNames = state.getVariableNames();
    std::vector<Interpreter::Value> values;
    std::vector<std::string> displayNames;

    for (const auto& name : varNames) {
        // 跳过内部变量
        if (name != "__output") {
            displayNames.push_back(name);
            values.push_back(state.getVariable(name));
        }
    }

    variableWidget_->updateVariables(displayNames, values);

    // 更新控制台输出
    consoleWidget_->setOutput(QString::fromStdString(state.getConsoleOutput()));

    // 高亮当前行
    int line = state.getLineNumber();
    if (line > 0) {
        editorWidget_->highlightLine(line, QColor(255, 255, 200)); // 浅黄色
    } else {
        editorWidget_->clearHighlights();
    }
}

void MainWindow::highlightCurrentLine(int line) {
    if (line > 0) {
        editorWidget_->highlightLine(line, QColor(255, 255, 200));
    }
}

void MainWindow::clearHighlights() {
    editorWidget_->clearHighlights();
}

void MainWindow::startExecution() {
    if (isExecuting_) return;

    // 解析并执行代码
    QString code = editorWidget_->getCode();
    if (code.isEmpty()) return;

    try {
        // 词法分析
        Interpreter::Tokenizer tokenizer(code.toStdString());
        auto tokens = tokenizer.tokenize();
        if (tokenizer.hasError()) {
            consoleWidget_->appendError(QString::fromStdString(tokenizer.getError()));
            return;
        }

        // 语法分析
        Interpreter::Parser parser(tokens);
        auto program = parser.parseProgram();
        if (parser.hasError()) {
            consoleWidget_->appendError(QString::fromStdString(parser.getError()));
            return;
        }

        // 设置程序并开始执行
        interpreter_->setProgram(std::move(program));

        // 开始执行
        isExecuting_ = true;
        executionTimer_->start();
        interpreter_->run();

    } catch (const std::exception& e) {
        consoleWidget_->appendError(QString::fromStdString(e.what()));
        isExecuting_ = false;
    }
}

void MainWindow::stopExecution() {
    if (isExecuting_) {
        executionTimer_->stop();
        isExecuting_ = false;
        interpreter_->pause();
    }
}