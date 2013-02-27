#ifndef GENFILEDOWNLOADER_H
#define GENFILEDOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QDateTime>

class QNetworkAccessManager;
class GenOAuthRequest;
class QFile;
class GenAppModel;

class GenFileDownloader : public QObject
{
    Q_OBJECT
public:
    GenFileDownloader(QObject *parent = 0);
    ~GenFileDownloader();
    void download(const QString& fileName, QDateTime lastModified, GenOAuthRequest *request);

private:
    void setLastModified(QString fileName, QDateTime lastModified);

public slots:
    void handleModelChanged(GenAppModel &model);
    void cancelDownload();

protected slots:
    void handleDownloadFinished();
    void handleDataReady();

signals:
    void downloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadFinished(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply);
    void downloadStarted(const QString& filename);

private:
    QNetworkAccessManager *_networkAccessmanager;
    QNetworkReply *_reply;
    QFile *_downloadedFile;
    bool _isAborted;
    bool _isInProgress;
    QString _downloadPath;
    QDateTime _lastModified;
};

#endif // GENFILEDOWNLOADER_H
