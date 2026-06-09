#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLineEdit>

class FilterProxyModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void importExcel();
    void onSearchTextChanged(const QString &text);
    void addRow();
    void deleteSelected();

private:
    void setupUi();
    QTableView *tableView;
    QStandardItemModel *model;
    FilterProxyModel *proxy; // 自定义代理，用于跨列搜索
    QPushButton *importBtn;
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QLineEdit *searchEdit;
};

#endif // MAINWINDOW_H
