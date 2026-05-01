#include "controlwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QStyle>
#include <QDebug>

ControlWidget::ControlWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void ControlWidget::setupUI() {
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(10);

    // 第一行：按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    // 创建按钮
    runButton_ = new QPushButton("运行", this);
    stepButton_ = new QPushButton("单步", this);
    resetButton_ = new QPushButton("重置", this);
    pauseButton_ = new QPushButton("暂停", this);

    // 设置按钮大小
    runButton_->setFixedWidth(80);
    stepButton_->setFixedWidth(80);
    resetButton_->setFixedWidth(80);
    pauseButton_->setFixedWidth(80);

    // 添加按钮到布局
    buttonLayout->addWidget(runButton_);
    buttonLayout->addWidget(stepButton_);
    buttonLayout->addWidget(resetButton_);
    buttonLayout->addWidget(pauseButton_);
    buttonLayout->addStretch();

    // 第二行：时间轴滑块
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->setSpacing(10);

    QLabel* sliderLabel = new QLabel("时间轴:", this);
    timelineSlider_ = new QSlider(Qt::Horizontal, this);
    timelineSlider_->setRange(0, 100);
    timelineSlider_->setValue(0);
    timelineSlider_->setEnabled(false);

    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(timelineSlider_, 1); // 滑块占据剩余空间

    // 第三行：状态信息
    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->setSpacing(20);

    statusLabel_ = new QLabel("状态: 就绪", this);
    stepLabel_ = new QLabel("步骤: 0/0", this);

    statusLayout->addWidget(statusLabel_);
    statusLayout->addWidget(stepLabel_);
    statusLayout->addStretch();

    // 添加到主布局
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addLayout(statusLayout);

    // 连接信号
    connect(runButton_, &QPushButton::clicked,
            this, &ControlWidget::onRunClicked);
    connect(stepButton_, &QPushButton::clicked,
            this, &ControlWidget::onStepClicked);
    connect(resetButton_, &QPushButton::clicked,
            this, &ControlWidget::onResetClicked);
    connect(pauseButton_, &QPushButton::clicked,
            this, &ControlWidget::onPauseClicked);
    connect(timelineSlider_, &QSlider::valueChanged,
            this, &ControlWidget::onSliderValueChanged);
    connect(timelineSlider_, &QSlider::sliderPressed,
            this, &ControlWidget::onSliderPressed);
    connect(timelineSlider_, &QSlider::sliderReleased,
            this, &ControlWidget::onSliderReleased);

    // 初始状态
    setRunningState(false);
    setPausedState(false);
    setFinishedState(false);
}

void ControlWidget::setStatusText(const QString& text) {
    statusLabel_->setText("状态: " + text);
}

void ControlWidget::setStepText(const QString& text) {
    stepLabel_->setText("步骤: " + text);
}

void ControlWidget::setStepCount(int current, int total) {
    QString text = QString("%1/%2").arg(current).arg(total);
    stepLabel_->setText("步骤: " + text);

    // 更新滑块
    if (total > 0) {
        timelineSlider_->setRange(0, total);
        timelineSlider_->setValue(current);
        timelineSlider_->setEnabled(true);
    } else {
        timelineSlider_->setRange(0, 0);
        timelineSlider_->setValue(0);
        timelineSlider_->setEnabled(false);
    }
}

void ControlWidget::setRunningState(bool running) {
    isRunning_ = running;

    runButton_->setEnabled(!running);
    stepButton_->setEnabled(!running);
    pauseButton_->setEnabled(running);
    resetButton_->setEnabled(true);

    if (running) {
        setStatusText("运行中");
        pauseButton_->setText("暂停");
    }
}

void ControlWidget::setPausedState(bool paused) {
    isPaused_ = paused;

    if (paused) {
        setStatusText("已暂停");
        pauseButton_->setText("继续");
        runButton_->setEnabled(true);
        stepButton_->setEnabled(true);
    } else {
        pauseButton_->setText("暂停");
    }
}

void ControlWidget::setFinishedState(bool finished) {
    isFinished_ = finished;

    if (finished) {
        setStatusText("已完成");
        runButton_->setEnabled(false);
        stepButton_->setEnabled(false);
        pauseButton_->setEnabled(false);
        resetButton_->setEnabled(true);
    }
}

void ControlWidget::onRunClicked() {
    if (!isRunning_) {
        emit runClicked();
    }
}

void ControlWidget::onStepClicked() {
    if (!isRunning_ && !isFinished_) {
        emit stepClicked();
    }
}

void ControlWidget::onResetClicked() {
    emit resetClicked();
}

void ControlWidget::onPauseClicked() {
    if (isRunning_) {
        emit pauseClicked();
    }
}

void ControlWidget::onSliderValueChanged(int value) {
    emit sliderValueChanged(value);
}

void ControlWidget::onSliderPressed() {
    emit sliderPressed();
}

void ControlWidget::onSliderReleased() {
    emit sliderReleased();
}