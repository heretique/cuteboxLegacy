#include <QFileSystemModel>

#include "genfilesystemfilterproxymodel.h"

GenFileSystemFilterProxyModel::GenFileSystemFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool GenFileSystemFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
#ifdef Q_OS_SYMBIAN
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
    return (fileModel->fileName(index0) != "D:" && fileModel->fileName(index0) != "Z:");
#else
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
#endif
}
