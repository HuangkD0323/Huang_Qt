#include "mainwindow.h"
#include "filterproxymodel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <algorithm>

// Detect QXlsx availability
#if defined(__has_include)
  #if __has_include(<QXlsx/Document>)
    #include <QXlsx/Document>
    #define HAS_QTXLSX 1
  #endif
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      tableView(new QTableView(this)),
      model(new QStandardItemModel(this)),
      proxy(new FilterProxyModel(this)),
      importBtn(new QPushButton(tr("导入 Excel/CSV"), this)),
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

static QString trimQuotes(const QString &s)
{
    QString t = s;
    if (t.startsWith('"') && t.endsWith('"') && t.length() >= 2)
        t = t.mid(1, t.length()-2);
    return t;
}

// Simple CSV parser for fallback (comma-separated, supports quoted fields)
static QStringList parseCsvLine(const QString &line)
{
    QStringList fields;
    QString cur;
    bool inQuotes = false;
    for (int i = 0; i < line.length(); ++i) {
        QChar ch = line[i];
        if (ch == '"') {
            if (inQuotes && i+1 < line.length() && line[i+1] == '"') {
                // escaped quote
                cur.append('"');
                ++i; // skip next quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.append(trimQuotes(cur));
            cur.clear();
        } else {
            cur.append(ch);
        }
    }
    fields.append(trimQuotes(cur));
    return fields;
}

void MainWindow::importExcel()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择文件"), QString(), tr("Excel/CSV Files (*.xlsx *.xls *.csv)"));
    if (file.isEmpty())
        return;

    // Prefer .xlsx via QXlsx if available
    if (file.endsWith(".csv", Qt::CaseInsensitive)) {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("读取失败"), tr("无法打开 CSV 文件。"));
            return;
        }
        model->clear();
        QTextStream in(&f);
        bool firstLine = true;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList cols = parseCsvLine(line);
            if (firstLine) {
                model->setColumnCount(cols.size());
                model->setHorizontalHeaderLabels(cols);
                firstLine = false;
            } else {
                QList<QStandardItem*> items;
                for (const QString &c : cols)
                    items.append(new QStandardItem(c));
                model->appendRow(items);
            }
        }
        f.close();
        return;
    }

#if HAS_QTXLSX
    // Use QXlsx to read .xlsx files
    if (file.endsWith(".xlsx", Qt::CaseInsensitive)) {
        try {
            QXlsx::Document xlsx(file);
            QXlsx::CellRange range = xlsx.dimension();
            if (!range.isValid()) {
                QMessageBox::warning(this, tr("读取失败"), tr("未检测到数据或无效的 xlsx 文件。"));
                return;
            }
            int rowCount = range.rowCount();
            int colCount = range.columnCount();

            model->clear();
            // read header
            QStringList headers;
            for (int c = 1; c <= colCount; ++c) {
                QVariant v = xlsx.read(1, c);
                headers << (v.isNull() ? QString("列%1").arg(c) : v.toString());
            }
            model->setHorizontalHeaderLabels(headers);

            for (int r = 2; r <= rowCount; ++r) {
                QList<QStandardItem*> items;
                for (int c = 1; c <= colCount; ++c) {
                    QVariant v = xlsx.read(r, c);
                    items.append(new QStandardItem(v.isNull() ? QString() : v.toString()));
                }
                model->appendRow(items);
            }
            return;
        } catch (...) {
            QMessageBox::warning(this, tr("错误"), tr("读取 xlsx 文件时发生异常。"));
            return;
        }
    }
#endif

    // If we reach here, .xls or .xlsx without QXlsx available (or .xls selected)
    QMessageBox::information(this, tr("提示"), tr("要导入 .xlsx 文件，请在你的环境中安装 QXlsx 库（或使用 CSV）。我已在 README 中说明如何安装 QXlsx，或者你可以在仓库中切换到包含 QXlsx 的分支。"));
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        proxy->setFilterRegularExpression(QRegularExpression());
    } else {
        QRegularExpression re(QRegularExpression::escape(text), QRegularExpression::CaseInsensitiveOption);
        proxy->setFilterRegularExpression(re);
    }
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
