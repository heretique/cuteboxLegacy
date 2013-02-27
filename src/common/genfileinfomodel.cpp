#include <qalgorithms.h>
#include <QTimer>
#include "genfileinfomodel.h"
#include "genfileinfo.h"
#include "genappmodel.h"
#include "gendataengine.h"
#include "genliterals.h"
#include "genutils.h"
#include "genlog.h"



GenFileInfoModel::GenFileInfoModel(GenAppModel &appModel, QObject *parent) :
    QAbstractListModel(parent),
    _appModel(appModel),
    _thumbRetrievalInProgress(false)
{

}

GenFileInfoModel::~GenFileInfoModel()
{
    qDeleteAll(_filesInfo);
}

int GenFileInfoModel::rowCount(const QModelIndex &parent) const
{
    return _filesInfo.size();
}

QVariant GenFileInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _filesInfo.size())
        return QVariant();


    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        const GenFileInfo * fileInfo = _filesInfo.at(index.row());
        QString name = fileInfo->_path;
        name = name.right(name.length() - name.lastIndexOf('/') - 1);
        if (!fileInfo->_isDir)
        {
            name.append("\n");
            name.append(FileSize(fileInfo->_size));
            name.append(", ");
            name.append(Duration(fileInfo->_modifiedTime));
        }
        return name;
    }
    else if (role == Qt::DecorationRole)
    {
        GenFileInfo *fi = _filesInfo.at(index.row());
        QPixmap icon = QPixmap();
        if (QPixmapCache::find(fi->_iconKey, &icon))
        {
            return icon;
        }
        else
        {
            if (_appModel.autoGetTumbs() && fi->_hasThumb && !_filesInfoMap.contains(fi->_path))
            {
                fi->_iconKey = QPixmapCache::Key();
                _filesInfoMap.insert(fi->_path, fi);
                startThumbnailGeneration();
            }
            return fi->_icon;
        }
    }
    else
        return QVariant();
}

QVariant GenFileInfoModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

void GenFileInfoModel::setFilesInfo(QList<GenFileInfo *> filesInfo)
{
    qDeleteAll(_filesInfo);
    _filesInfo.clear();
    _filesInfoMap.clear();
    _filesInfo = filesInfo;
    sort(0);
    reset();

    QListIterator<GenFileInfo*> it(_filesInfo);
    GenFileInfo *fileInfo = NULL;
    while(it.hasNext())
    {
        fileInfo = it.next();
        if (fileInfo->_hasThumb && _appModel.autoGetTumbs())
        {
            QByteArray thumbnail = _appModel.dbEngine().retrieveThumbnail(thumbnailHash(fileInfo->_path));
            if (!thumbnail.isNull())
            {
                QImage image;
                image.loadFromData(thumbnail, "JPEG");
                fileInfo->_iconKey = QPixmapCache::insert(QPixmap::fromImage(image));
            }
            else
                _filesInfoMap.insert(fileInfo->_path, fileInfo);
        }
    }
}

void GenFileInfoModel::addFileInfo(GenFileInfo *fileInfo)
{
    _filesInfo.append(fileInfo);
    if (_appModel.autoGetTumbs() && fileInfo->_hasThumb && !_filesInfoMap.contains(fileInfo->_path))
    {
        fileInfo->_iconKey = QPixmapCache::Key();
        _filesInfoMap.insert(fileInfo->_path, fileInfo);
        startThumbnailGeneration();
    }
    sort(0);
    reset();
}

const GenFileInfo *GenFileInfoModel::fileInfo(const QModelIndex &index) const
{
    if (!index.isValid())
        return NULL;

    if (index.row() >= _filesInfo.size())
        return NULL;


    return _filesInfo.at(index.row());
}

void GenFileInfoModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)
    qSort(_filesInfo.begin(), _filesInfo.end(), fileInfoLessThan);
}

void GenFileInfoModel::handleThumbnailReady(QString path, QImage thumbnail)
{
    QLOG_DEBUG("GenFileInfoModel::handleThumbnailReady");
    GenFileInfo *fi = _filesInfoMap.value(path, NULL);
    if (fi)
    {
        fi->_icon = QPixmap();
        fi->_iconKey = QPixmapCache::insert(QPixmap::fromImage(thumbnail));
        _filesInfoMap.remove(path);
        reset();
    }
    requestNextThumbnail();
}

void GenFileInfoModel::handleThumbnailError(QString path, QNetworkReply::NetworkError error,
                                            int httpStatus,
                                            QByteArray reply)
{
    QLOG_DEBUG("GenFileInfoModel::handleThumbnailError - error: " << error
               << ", httpStatus: " << httpStatus
               << ", reply: " << reply);
    _filesInfoMap.remove(path);
    requestNextThumbnail();
}

void GenFileInfoModel::startThumbnailGeneration() const
{
    QLOG_DEBUG("GenFileInfoModel::startThumbnailGeneration");
    if (!_thumbRetrievalInProgress)
    {
        _thumbRetrievalInProgress = true;
        requestNextThumbnail();
    }
}

void GenFileInfoModel::requestNextThumbnail() const
{
    QLOG_DEBUG("GenFileInfoModel::requestNextThumbnail()");
    QMap<QString, GenFileInfo*>::const_iterator it = _filesInfoMap.constBegin();
    if (it != _filesInfoMap.constEnd())
    {
        QLOG_DEBUG("GenFileInfoModel::requestNextThumbnail() - 1");
        emit requestThumbnail(it.key());
        QLOG_DEBUG("GenFileInfoModel::requestNextThumbnail() - 2");
    }
    else
    {
        _thumbRetrievalInProgress = false;
    }
    QLOG_DEBUG("GenFileInfoModel::requestNextThumbnail() - 3");
}
