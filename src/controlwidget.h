#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit ControlWidget(QWidget* parent = nullptr);

    // 获取UI部件
    QPushButton* getRunButton() const { return runButton_; }
    QPushButton* getStepButton() const { return stepButton_; }
    QPushButton* getResetButton() const { return resetButton_; }
    QPushButton* getPauseButton() const { return pauseButton_; }
    QSlider* getTimelineSlider() const { return timelineSlider_; }
    QLabel* getStatusLabel() const { return statusLabel_; }
    QLabel* getStepLabel() const { return stepLabel_; }

    // 设置状态
    void setStatusText(const QString& text);
    void setStepText(const QString& text);
    void setStepCount(int current, int total);

    // 设置按钮状态
    void setRunningState(bool running);
    void setPausedState(bool paused);
    void setFinishedState(bool finished);

signals:
    void runClicked();
    void stepClicked();
    void resetClicked();
    void pauseClicked();
    void sliderValueChanged(int value);
    void sliderPressed();
    void sliderReleased();

private slots:
    void onRunClicked();
    void onStepClicked();
    void onResetClicked();
    void onPauseClicked();
    void onSliderValueChanged(int value);
    void onSliderPressed();
    void onSliderReleased();

private:
    // 初始化UI
    void setupUI();

    // 成员变量
    QPushButton* runButton_ = nullptr;
    QPushButton* stepButton_ = nullptr;
    QPushButton* resetButton_ = nullptr;
    QPushButton* pauseButton_ = nullptr;
    QSlider* timelineSlider_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* stepLabel_ = nullptr;

    // 状态标志
    bool isRunning_ = false;
    bool isPaused_ = false;
    bool isFinished_ = false;
};

#endif // CONTROLWIDGET_H