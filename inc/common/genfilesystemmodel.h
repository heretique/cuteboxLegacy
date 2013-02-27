#ifndef GENFILESYSTEMMODEL_H
#define GENFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include "genfileiconprovider.h"

class GenFileSystemModel : public QFileSystemModel
{
public:
    explicit GenFileSystemModel(QObject *parent = 0);
    ~GenFileSystemModel();
    QVariant data(const QModelIndex &index, int role) const;

private:
    GenFileIconProvider *_iconProvider;
};

#endif // GENFILESYSTEMMODEL_H
