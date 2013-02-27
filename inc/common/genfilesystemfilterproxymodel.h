#ifndef GENFILESYSTEMFILTERPROXYMODEL_H
#define GENFILESYSTEMFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class GenFileSystemFilterProxyModel : public QSortFilterProxyModel
{
public:
    GenFileSystemFilterProxyModel(QObject *parent = 0);

protected: // FROM QSortFilterProxyModel
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};


#endif // GENFILESYSTEMFILTERPROXYMODEL_H
