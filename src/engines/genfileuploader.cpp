#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include "genfileuploader.h"
#include "genoauthrequest.h"
#include "genupfile.h"
#include "genlog.h"

GenFileUploader::GenFileUploader(QObject *parent)
    : QObject(parent),
    _networkAccessmanager(new QNetworkAccessManager(this)),
    _reply(NULL),
    _uploadFile(NULL),
    _isAborted(false),
    _isInProgress(false)
{

}

GenFileUploader::~GenFileUploader()
{
    delete _reply;
}

void GenFileUploader::upload(const QString &fileName, GenOAuthRequest *request)
{

    if (request == 0) {
        QLOG_WARN("Request is NULL. Cannot proceed.");
        return;
    }

    if (!request->requestEndpoint().isValid()) {
        QLOG_WARN("Request endpoint URL is not valid. Cannot proceed.");
        return;
    }

    if (!request->isValid()) {
        QLOG_WARN("Request is not valid. Cannot proceed.");
        return;
    }

    QNetworkRequest networkRequest;
    // Set the request's URL to the OAuth request's endpoint.
    networkRequest.setUrl(request->requestEndpoint());
    QLOG_DEBUG("GenFileUploader::upload - url = " << request->requestEndpoint());
    // And now fill the request with "Authorization" header data.
    QList<QByteArray> requestHeaders = request->requestParameters();
    QByteArray authHeader;

    bool first = true;
    foreach (const QByteArray header, requestHeaders)
    {
        if (!first) {
            authHeader.append(", ");
        }
        else {
            authHeader.append("OAuth ");
            first = false;
        }

        authHeader.append(header);
    }
    networkRequest.setRawHeader("Authorization", authHeader);

    if (request->httpMethod() != HttpPOST)
    {
        QLOG_FATAL("GenFileUploader::upload - encountered GET HTTP method");
    }


    _isAborted = false;
    QString shortFilename = fileName.mid(fileName.lastIndexOf("/") + 1);
    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    networkRequest.setRawHeader("Accept-Encoding", "gzip,deflate");
    networkRequest.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata1("--"+boundaryRegular+"\r\n");
    mimedata1.append("Content-Disposition: form-data; name=\"file\"; filename=\"" + shortFilename.toUtf8()+"\"\r\n");
    mimedata1.append("Content-Type: application/octet-stream\r\n\r\n");

    QByteArray mimedata2(boundaryLast);

    delete _uploadFile;
    _uploadFile = new GenUpFile(fileName, mimedata1, mimedata2, this);
    if (_uploadFile->openFile())
    {
        _reply = _networkAccessmanager->post(networkRequest, _uploadFile);
        connect(_reply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(uploadProgress(qint64,qint64)));
        connect(_reply, SIGNAL(finished()), SLOT(handleUploadFinished()));

        _isInProgress = true;
        emit uploadStarted(shortFilename);
    }
    else
    {
        emit uploadFinished(true, false, tr("Error: can't open file %1").arg(fileName));
    }
}

void GenFileUploader::handleUploadFinished()
{
    _isInProgress = false;
    if (_uploadFile)
    {
        _uploadFile->close();
        delete _uploadFile;
        _uploadFile = 0;
    }

    if (_isAborted)
    {
        emit uploadFinished(false, true, QString());
    }
    else if (_reply->error()>0)
    {
        emit uploadFinished(true, false,
                            QString("Network error: %1").arg(QString::number(_reply->error())));
        QLOG_DEBUG("--Reply: " << _reply->readAll());
    }
    else
    {
        QVariant statusCodeVariant = _reply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute);
        if (!statusCodeVariant.isNull())
        {
            int statusCode = statusCodeVariant.toInt();
            emit uploadFinished(false, false, QString("HTTP Status code: %1").arg(statusCode));
        }
        else
        {
            QLOG_DEBUG("HTTPEngine::finished  HTTP ERROR 1");
            emit uploadFinished(true, false, _reply->errorString());
            QLOG_DEBUG("--Reply: " << _reply->readAll());
        }
    }

    _reply->deleteLater();
    _reply = 0;
}

void GenFileUploader::cancelUpload()
{
    if (_reply)
        _reply->abort();
}
