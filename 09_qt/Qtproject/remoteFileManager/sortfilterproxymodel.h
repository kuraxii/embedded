#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QSortFilterProxyModel *parent = nullptr);
protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
signals:

};

#endif // SORTFILTERPROXYMODEL_H
