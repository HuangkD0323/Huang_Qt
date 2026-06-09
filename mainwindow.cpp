#include "mainwindow.h"
#include "filterproxymodel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QDebug>

#include <QAxObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      tableView(new QTableView(this)),
      model(new QStandardItemModel(this)),
      proxy(new FilterProxyModel(this)),
      importBtn(new QPushButton(tr("导入 Excel"), this)),
      addBtn(new QPushButton(tr("新增行"), this)),
      deleteBtn(new QPushButton(tr("删除选中"), this)),
      searchEdit(new QLineEdit(this))
{
    setupUi();

    connect(importBtn, &QPushButton::clicked, this, &MainWindow::importExcel);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addRow);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteSelected);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    proxy->setSourceModel(model);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    tableView->setModel(proxy);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

    searchEdit->setPlaceholderText(tr("输入关键字检索（跨列）"));

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(importBtn);
    hl->addWidget(addBtn);
    hl->addWidget(deleteBtn);
    hl->addStretch();
    hl->addWidget(searchEdit);

    QVBoxLayout *vl = new QVBoxLayout;
    vl->addLayout(hl);
    vl->addWidget(tableView);

    central->setLayout(vl);
}

void MainWindow::importExcel()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择 Excel 文件"), QString(), tr("Excel Files (*.xlsx *.xls)"));
    if (file.isEmpty())
        return;

    QAxObject *excel = nullptr;
    QAxObject *workbooks = nullptr;
    QAxObject *workbook = nullptr;
    QAxObject *sheet = nullptr;
    try {
        excel = new QAxObject("Excel.Application", this);
        excel->setProperty("Visible", false);
        workbooks = excel->querySubObject("Workbooks");
        workbook = workbooks->querySubObject("Open(const QString&)", file);
        sheet = workbook->querySubObject("Worksheets(int)", 1); // 读取第一个工作表

        QAxObject *usedRange = sheet->querySubObject("UsedRange");
        QAxObject *rows = usedRange->querySubObject("Rows");
        QAxObject *cols = usedRange->querySubObject("Columns");
        int rowCount = rows->property("Count").toInt();
        int colCount = cols->property("Count").toInt();

        if (rowCount <= 0 || colCount <= 0) {
            QMessageBox::warning(this, tr("读取失败"), tr("未检测到数据。"));
            workbook->dynamicCall("Close()");
            excel->dynamicCall("Quit()");
            delete excel;
            return;
        }

        model->clear();

        // 第一行作为表头
        QStringList headers;
        for (int c = 1; c <= colCount; ++c) {
            QAxObject *cell = sheet->querySubObject("Cells(int,int)", 1, c);
            QVariant val = cell->property("Value");
            QString h = val.isNull() ? QString("列%1").arg(c) : val.toString();
            headers << h;
            delete cell;
        }
        model->setHorizontalHeaderLabels(headers);

        // 数据行（从第二行开始）
        for (int r = 2; r <= rowCount; ++r) {
            QList<QStandardItem*> items;
            for (int c = 1; c <= colCount; ++c) {
                QAxObject *cell = sheet->querySubObject("Cells(int,int)", r, c);
                QVariant val = cell->property("Value");
                QStandardItem *it = new QStandardItem(val.isNull() ? QString() : val.toString());
                items.append(it);
                delete cell;
            }
            model->appendRow(items);
        }

        // 关闭 workbook & excel
        workbook->dynamicCall("Close(bool)", false);
        excel->dynamicCall("Quit()");
        delete excel;
    } catch (...) {
        if (workbook) workbook->dynamicCall("Close(bool)", false);
        if (excel) { excel->dynamicCall("Quit()"); delete excel; }
        QMessageBox::critical(this, tr("错误"), tr("读取 Excel 时发生异常，请确保系统已安装 Excel 并允许 COM 访问。"));
    }
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::FixedString);
    proxy->setFilterRegExp(regExp);
}

void MainWindow::addRow()
{
    int cols = model->columnCount();
    if (cols == 0) {
        bool ok;
        int colCount = QInputDialog::getInt(this, tr("列数"), tr("请输入表格列数："), 3, 1, 100, 1, &ok);
        if (!ok) return;
        QStringList headers;
        for (int i = 0; i < colCount; ++i) {
            QString name = QInputDialog::getText(this, tr("列名"), tr("请输入第%1列名：").arg(i+1), QLineEdit::Normal, QString("列%1").arg(i+1), &ok);
            if (!ok) return;
            headers << name;
        }
        model->setColumnCount(colCount);
        model->setHorizontalHeaderLabels(headers);
        cols = colCount;
    }

    QList<QStandardItem*> items;
    for (int c = 0; c < cols; ++c)
        items.append(new QStandardItem(QString()));
    model->appendRow(items);
    QModelIndex idx = model->index(model->rowCount()-1, 0);
    tableView->scrollTo(proxy->mapFromSource(idx));
    tableView->setCurrentIndex(proxy->mapFromSource(idx));
    tableView->edit(proxy->mapFromSource(model->index(model->rowCount()-1, 0)));
}

void MainWindow::deleteSelected()
{
    QModelIndexList sels = tableView->selectionModel()->selectedRows();
    if (sels.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先选择要删除的行。"));
        return;
    }
    QList<int> rows;
    for (const QModelIndex &idx : sels) {
        QModelIndex src = proxy->mapToSource(idx);
        rows.append(src.row());
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (int r : rows) {
        model->removeRow(r);
    }
}
