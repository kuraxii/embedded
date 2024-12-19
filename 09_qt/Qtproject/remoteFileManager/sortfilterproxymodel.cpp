#include "sortfilterproxymodel.h"
#include <QDebug>
SortFilterProxyModel::SortFilterProxyModel(QSortFilterProxyModel *parent) : QSortFilterProxyModel(parent)
{




}

bool SortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // 获取两个比较项的文件类型
            QString leftType = sourceModel()->data(left, Qt::UserRole + 1).toString();
            QString rightType = sourceModel()->data(right, Qt::UserRole + 1).toString();

            // 将文件夹排在前面
            if (leftType == "文件夹" && rightType != "文件夹") {
                return false;
            } else if (leftType != "文件夹" && rightType == "文件夹") {
                return false;
            }

            // 如果类型相同，则按原始顺序（或其他标准）排序
            return QSortFilterProxyModel::lessThan(left, right);
}
