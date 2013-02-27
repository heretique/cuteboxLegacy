#ifndef GENFILEUPLOADER_H
#define GENFILEUPLOADER_H

#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
class GenOAuthRequest;
class GenUpFile;

class GenFileUploader : public QObject
{
    Q_OBJECT
public:
    GenFileUploader(QObject *parent = 0);
    ~GenFileUploader();
    void upload(const QString& fileName, GenOAuthRequest *request);
    void cancelUpload();

protected slots:
    void handleUploadFinished();

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(bool error, bool aborted, const QString &text);
    void uploadStarted(const QString& filename);

private:
    QNetworkAccessManager *_networkAccessmanager;
    QNetworkReply *_reply;
    GenUpFile *_uploadFile;
    bool _isAborted;
    bool _isInProgress;
};

#endif // GENFILEUPLOADER_H
