#include <QBuffer>
#include "genlog.h"

#include "genthumbnailmanager.h"
#include "genthumbnaildownloader.h"
#include "genappmodel.h"
#include "gendataengine.h"
#include "genutils.h"


GenThumbnailManager::GenThumbnailManager(GenAppModel *appModel, QObject *parent)
    :QThread(parent),
      _appModel(appModel),
      _thumbnailDownloader(NULL)
{
    moveToThread(this);
}

GenThumbnailManager::~GenThumbnailManager()
{
    delete _thumbnailDownloader;
    _thumbnailDownloader = NULL;
}

void GenThumbnailManager::init()
{
    _thumbnailDownloader = new GenThumbnailDownloader(this);
    connect(_thumbnailDownloader,
            SIGNAL(thumbnailReady(QByteArray)),
            SLOT(handleThumbnailReady(QByteArray)));
    connect(_thumbnailDownloader,
            SIGNAL(thumbnailError(QNetworkReply::NetworkError,int,QByteArray)),
            SLOT(handleThumbnailError(QNetworkReply::NetworkError,int,QByteArray)));
    _thumbnailDownloader->handleModelChanged(*_appModel);
}

void GenThumbnailManager::run()
{
    init();
    exec();
    delete _thumbnailDownloader;
    _thumbnailDownloader = NULL;
}

void GenThumbnailManager::clear()
{
    delete _thumbnailDownloader;
    _thumbnailDownloader = 0;
}

void GenThumbnailManager::handleThumbnailRequest(QString path)
{
    QLOG_DEBUG("GenThumbnailManager::handleThumbnailRequest");
    _workingPath = path;
    QByteArray thumbnail = _appModel->dbEngine().retrieveThumbnail(thumbnailHash(path));
    if (!thumbnail.isNull())
    {
        QImage image;
        image.loadFromData(thumbnail, "JPEG");
        emit thumbnailReady(_workingPath, image);
    }
    else
        _thumbnailDownloader->getThumbnail(path);
}

void GenThumbnailManager::handleThumbnailReady(QByteArray thumbnail)
{
    QLOG_DEBUG("GenThumbnailManager::handleThumbnailReady");
    _appModel->dbEngine().replaceOrAddThumbnail(thumbnailHash(_workingPath), thumbnail);
    QImage image;
    image.loadFromData(thumbnail, "JPEG");
    emit thumbnailReady(_workingPath, image);
}

void GenThumbnailManager::handleThumbnailError(QNetworkReply::NetworkError error,
                                               int httpStatus,
                                               QByteArray reply)
{
    QLOG_DEBUG("GenThumbnailManager::handleThumbnailError");
    emit thumbnailError(_workingPath, error, httpStatus, reply);
}
