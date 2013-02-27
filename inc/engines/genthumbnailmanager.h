#ifndef GENTHUMBNAILMANAGER_H
#define GENTHUMBNAILMANAGER_H

#include <QThread>
#include <QImage>
#include <QNetworkReply>

class GenThumbnailDownloader;
class GenAppModel;

class GenThumbnailManager : public QThread
{
    Q_OBJECT
public:
    GenThumbnailManager(GenAppModel *appModel, QObject *parent = 0);
    ~GenThumbnailManager();

protected:
    void init();
    void run();
    void clear();

public slots:
    void handleThumbnailRequest(QString path);

private slots:
    void handleThumbnailReady(QByteArray thumbnail);
    void handleThumbnailError(QNetworkReply::NetworkError error,
                              int httpStatus,
                              QByteArray reply);
signals:
    void thumbnailReady(QString path, QImage thumbnail);
    void thumbnailError(QString path, QNetworkReply::NetworkError error,
                              int httpStatus,
                              QByteArray reply);
private:
    QString _workingPath;
    GenAppModel *_appModel;
    GenThumbnailDownloader *_thumbnailDownloader;
};


#endif // GENTHUMBNAILMANAGER_H
