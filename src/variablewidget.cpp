#include "variablewidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

VariableWidget::VariableWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void VariableWidget::setupUI() {
    // 创建主布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建表格部件
    tableWidget_ = new QTableWidget(this);
    tableWidget_->setColumnCount(3); // 名称、值、类型
    tableWidget_->setHorizontalHeaderLabels(QStringList() << "变量名" << "值" << "类型");
    tableWidget_->setAlternatingRowColors(true);
    tableWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置表格样式
    tableWidget_->horizontalHeader()->setStretchLastSection(true);
    tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget_->verticalHeader()->setVisible(false);
    tableWidget_->setShowGrid(true);

    // 设置初始行数
    tableWidget_->setRowCount(0);

    // 添加到布局
    layout->addWidget(tableWidget_);
}

void VariableWidget::updateVariables(const std::vector<std::string>& names,
                                    const std::vector<Interpreter::Value>& values) {
    if (names.size() != values.size()) {
        qWarning() << "VariableWidget: names and values size mismatch";
        return;
    }

    // 清空表格
    tableWidget_->setRowCount(0);

    // 添加新行
    for (size_t i = 0; i < names.size(); ++i) {
        int row = tableWidget_->rowCount();
        tableWidget_->insertRow(row);

        // 变量名
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(names[i]));
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget_->setItem(row, 0, nameItem);

        // 值
        QString valueStr = valueToString(values[i]);
        QTableWidgetItem* valueItem = new QTableWidgetItem(valueStr);
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget_->setItem(row, 1, valueItem);

        // 类型
        QString typeStr = typeToString(values[i]);
        QTableWidgetItem* typeItem = new QTableWidgetItem(typeStr);
        typeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget_->setItem(row, 2, typeItem);
    }

    // 调整列宽
    tableWidget_->resizeColumnsToContents();
}

void VariableWidget::clearVariables() {
    tableWidget_->setRowCount(0);
}

void VariableWidget::setVariables(const std::vector<std::pair<std::string, Interpreter::Value>>& vars) {
    // 清空表格
    tableWidget_->setRowCount(0);

    // 添加变量
    for (const auto& var : vars) {
        setVariable(var.first, var.second);
    }
}

void VariableWidget::setVariable(const std::string& name, const Interpreter::Value& value) {
    // 检查是否已存在同名变量
    for (int row = 0; row < tableWidget_->rowCount(); ++row) {
        QTableWidgetItem* item = tableWidget_->item(row, 0);
        if (item && item->text().toStdString() == name) {
            // 更新现有行
            QTableWidgetItem* valueItem = tableWidget_->item(row, 1);
            QTableWidgetItem* typeItem = tableWidget_->item(row, 2);

            if (valueItem) {
                valueItem->setText(valueToString(value));
            }
            if (typeItem) {
                typeItem->setText(typeToString(value));
            }
            return;
        }
    }

    // 添加新行
    int row = tableWidget_->rowCount();
    tableWidget_->insertRow(row);

    // 变量名
    QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(name));
    nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tableWidget_->setItem(row, 0, nameItem);

    // 值
    QString valueStr = valueToString(value);
    QTableWidgetItem* valueItem = new QTableWidgetItem(valueStr);
    valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tableWidget_->setItem(row, 1, valueItem);

    // 类型
    QString typeStr = typeToString(value);
    QTableWidgetItem* typeItem = new QTableWidgetItem(typeStr);
    typeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tableWidget_->setItem(row, 2, typeItem);
}

void VariableWidget::removeVariable(const std::string& name) {
    for (int row = 0; row < tableWidget_->rowCount(); ++row) {
        QTableWidgetItem* item = tableWidget_->item(row, 0);
        if (item && item->text().toStdString() == name) {
            tableWidget_->removeRow(row);
            return;
        }
    }
}

QString VariableWidget::valueToString(const Interpreter::Value& value) const {
    try {
        return QString::fromStdString(value.toString());
    } catch (const std::exception& e) {
        qWarning() << "Failed to convert value to string:" << e.what();
        return "error";
    }
}

QString VariableWidget::typeToString(const Interpreter::Value& value) const {
    try {
        return QString::fromStdString(value.typeString());
    } catch (const std::exception& e) {
        qWarning() << "Failed to get value type:" << e.what();
        return "unknown";
    }
}