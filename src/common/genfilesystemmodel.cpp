#include <QLinearGradient>

#include "genfilesystemmodel.h"
#include "genconstants.h"

GenFileSystemModel::GenFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    _iconProvider = new GenFileIconProvider;
    setIconProvider(_iconProvider);
}

GenFileSystemModel::~GenFileSystemModel()
{
    delete _iconProvider;
}

QVariant GenFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    if (Qt::DisplayRole == role)
    {
        QString name = QFileSystemModel::data(index, role).toString();
        //        name = name.right(name.length() - name.lastIndexOf('/') - 1);
        if (!isDir(index))
        {
            name.append("\n");
            QModelIndex tmpIndex = createIndex(index.row(), 1, index.internalPointer());
            name.append(QFileSystemModel::data(tmpIndex, role).toString());
            name.append(", ");
            tmpIndex = createIndex(index.row(), 3, index.internalPointer());
            name.append(QFileSystemModel::data(tmpIndex, role).toString());
        }
        return name;
    }
    else if (role == Qt::BackgroundRole)
    {
        if (size(index) > FILE_SIZE_UPLOAD_LIMIT)
        {
            QLinearGradient gradient(0,0, 300, 0);
            gradient.setColorAt(0.0, QColor(255, 173, 173));
            gradient.setColorAt(1.0, QColor(238, 238, 238));
            return QBrush(gradient);
        }
        else
            return QFileSystemModel::data(index, role);
    }
    else
        return QFileSystemModel::data(index, role);
}
