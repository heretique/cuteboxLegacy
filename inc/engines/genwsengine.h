#ifndef _GENWSENGINE_H_
#define _GENWSENGINE_H_

#include <QObject>
#include <QPair>
#include <QVariantMap>
#include <QStringList>
#include <QNetworkReply>
#include "gentypes.h"

class GenHTTPEngine;
class QUrl;
class GenOAuthManager;
class GenOAuthRequest;
class GenWSRequestInfo;
class GenFileInfo;
class GenFileUploader;
class GenFileDownloader;
class GenAppModel;

class GenWSEngine: public QObject
{
    Q_OBJECT
public:

    /*!
     * \brief Constructor
     * \param parent, a Qobject argument. The parent object.
     */
    GenWSEngine(QObject *parent = 0);
    /*!
     * \brief Destructor
     */
    virtual ~GenWSEngine();

    /*!
     * \brief  returns the httpEngine
     * \return HTTPEngine
     */
    GenHTTPEngine* httpEngine();

    /*!
     * \brief Aborts all pending operations
     */
    void abortOperations();

    void setUserToken(const QString &token, const QString &secret);
    const QString token() const;
    const QString tokenSecret() const;

public:
    // requests
    void startWSRequest(const GenWSRequestInfo *request, const QByteArray &data);
    void startWSRequest(const GenWSRequestInfo *request, const QString &path,
                        const QList<QPair<QString, QString> > &params, const QByteArray &data);
    void startWSRequest(const GenWSRequestInfo *request, const QString &path,
                        const QByteArray &data);
    void uploadFiles(const QString &path, FilesList files);
    void uploadFile(const QString &path, QString file);
    void downloadFiles(FilesList files);
    void downloadFile(const QString& file, QDateTime lastModified);
    void copyOrMoveFiles(FilesList files, QString path, bool move = false);
    void copyFile(QString file, QString destination, bool move = false);
    void deleteFiles(FilesList files);
    void deleteFile(QString file);
    void createFolder(QString path);
    void cancelDownload();
    void cancelUpload();
private:

    /// HANDLERS
    void handleTokenReceived(const QVariant &variant);
    void handleMetaDataReceived(const QVariant &variant);
    void handleAccountInfoReceived(const QVariant &variant);
    void handleFileDeleted(const QVariant &variant);
    void handleFileCopiedOrMoved(const QVariant &variant);
    void handleFolderCreated(const QVariant &variant);

    // UTILITY
    GenFileInfo *convertFromVariant(const QString& root, const QVariantMap &variantMap);
    bool convertToBool(const QString &string);
    QDateTime convertToDateTime(const QString &string);
    GenAccountInfo accountInfoFromVariant(const QVariantMap &variantMap);
    QString errorFromVariant(const QVariantMap &variantMap);

public slots:
    void handleModelChanged(GenAppModel& model);

protected slots:
    /*!
     * \brief Slot, used to handle a successfull http response
     * \param response, an int argument. The response code.
     * \param data, a const QByteArray& argument. The response data.
     * \param url, a const QUrl&. The url of the response.
     */
    void handleResponseReceived(int statusCode, const QByteArray &data);
    void handleResponseError(QNetworkReply::NetworkError networkError,
                             int httpStatusCode,
                             QString errorString,
                             const QByteArray &data);
    /*!
     * \brief handle for the disease categories response
     * \param data, a const QByteArray&. The xml received.
     */
    void parseResponse(const QByteArray &data);

    void parseErrorResponse(int response, const QByteArray &data);

    void handleUploadFinished(bool error, bool aborted, const QString &text);
    void handleDownloadFinished(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply);

signals:
    void httpError();
    void accountCreated();
    void userTokenReceived(QString token, QString secret);
    void metaDataReceived(GenFileInfo *parent, QList<GenFileInfo*> content);
    void metaDataNotModified();
    void requestStarted();
    void responseReceived();
    void responseError(QString error);
    void uploadStarted(const QString& filename);
    void uploadFinished();
    void downloadStarted(const QString& filename);
    void downloadFinished();
    void fileProgress(qint64 bytesSent, qint64 bytesTotal);
    void totalProgress(int current, int total);
    void filesDeleted();
    void filesCopiedOrMoved();
    void accountInfoReceived(GenAccountInfo accountInfo);
    void folderCreated(GenFileInfo *fileInfo);

private:
    GenHTTPEngine *_httpEngine; /*!< Http engine */
    GenOAuthRequest *_oauthRequest;
    GenFileUploader *_fileUploader;
    GenFileDownloader *_fileDownloader;
    QString _oauthUserToken;
    QString _oauthUserSecret;
    int _currentWSRequestId;
    FilesList _filesList;
    int _currentProgress;
    int _totalProgress;
    bool _move;
    QString _destination;
};

#endif // _GENWSENGINE_H_
