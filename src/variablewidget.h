#ifndef VARIABLEWIDGET_H
#define VARIABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <string>
#include <vector>
#include "interpreter/value.h"

class VariableWidget : public QWidget {
    Q_OBJECT

public:
    explicit VariableWidget(QWidget* parent = nullptr);

    // 更新变量显示
    void updateVariables(const std::vector<std::string>& names,
                         const std::vector<Interpreter::Value>& values);

    // 清除所有变量
    void clearVariables();

    // 设置变量（批量）
    void setVariables(const std::vector<std::pair<std::string, Interpreter::Value>>& vars);

    // 添加或更新单个变量
    void setVariable(const std::string& name, const Interpreter::Value& value);

    // 移除变量
    void removeVariable(const std::string& name);

    // 获取表格部件（用于布局等）
    QTableWidget* getTableWidget() const { return tableWidget_; }

private:
    // 初始化UI
    void setupUI();

    // 工具方法
    QString valueToString(const Interpreter::Value& value) const;
    QString typeToString(const Interpreter::Value& value) const;

    // 成员变量
    QTableWidget* tableWidget_ = nullptr;
};

#endif // VARIABLEWIDGET_H