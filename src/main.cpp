#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    // 创建Qt应用程序
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("TimeTravelDebugger");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CourseProject");

    // 设置全局字体（确保跨平台一致性）
    QFont font("Monospace", 10);
    app.setFont(font);

    // 设置样式（使用系统默认样式）
    app.setStyle(QStyleFactory::create("Fusion"));

    // 创建并显示主窗口
    MainWindow mainWindow;
    mainWindow.show();

    // 运行应用程序
    return app.exec();
}