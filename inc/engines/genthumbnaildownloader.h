#ifndef GENTHUMBNAILDOWNLOADER_H
#define GENTHUMBNAILDOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QDateTime>

class QNetworkAccessManager;
class GenOAuthRequest;
class GenAppModel;

class GenThumbnailDownloader : public QObject
{
    Q_OBJECT
public:
    GenThumbnailDownloader(QObject *parent = 0);
    ~GenThumbnailDownloader();
    void getThumbnail(const QString& fileName);

public slots:
    void handleModelChanged(GenAppModel &model);
    void cancelThumbnailRetrieval();

protected slots:
    void handleDownloadFinished();
    void handleDataReady();

signals:
    void thumbnailReady(QByteArray thumbnail);
    void thumbnailError(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply);

private:
    QNetworkAccessManager *_networkAccessmanager;
    QNetworkReply *_reply;
    GenOAuthRequest *_oauthRequest;
    QByteArray _thumbnail;
    QString _oauthUserToken;
    QString _oauthUserSecret;
};


#endif // GENTHUMBNAILDOWNLOADER_H
