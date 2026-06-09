#ifndef FILTERPROXYMODEL_H
#define FILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

protected:
    // 重写以便在所有列中进行过滤
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        if (filterRegExp().isEmpty())
            return true;

        QAbstractItemModel *m = sourceModel();
        int cols = m->columnCount(source_parent);
        for (int c = 0; c < cols; ++c) {
            QModelIndex idx = m->index(source_row, c, source_parent);
            QVariant data = m->data(idx);
            if (data.toString().contains(filterRegExp()))
                return true;
        }
        return false;
    }
};

#endif // FILTERPROXYMODEL_H
