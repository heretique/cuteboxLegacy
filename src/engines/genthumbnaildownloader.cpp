#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QThread>

#include "genthumbnaildownloader.h"
#include "genoauthrequest.h"
#include "genliterals.h"
#include "genappmodel.h"
#include "gendropboxrequests.h"
#include "genlog.h"

GenThumbnailDownloader::GenThumbnailDownloader(QObject *parent)
    : QObject(parent),
      _networkAccessmanager(new QNetworkAccessManager(this)),
      _reply(NULL),
      _oauthRequest(new GenOAuthRequest(this))
{

}

GenThumbnailDownloader::~GenThumbnailDownloader()
{
    delete _reply;
    _reply = NULL;
}

void GenThumbnailDownloader::getThumbnail(const QString &fileName)
{
    QLOG_DEBUG("GenThumbnailDownloader::getThumbnail");
    const GenWSRequestInfo *request = GetDropboxRequestById(WSReqGetThumbnails);
    QString urlBase;
    if (request->_isSecure) {
        urlBase = "https://";
    }
    else {
        urlBase = "http://";
    }
    urlBase += request->_domain + DROPBOX_VERSION + request->_path;
    if (!fileName.isEmpty())
    {
        urlBase += fileName;
    }

    QUrl url = urlBase;
    _oauthRequest->initRequest(request->_authType, url);
    _oauthRequest->setHttpMethod(request->_httpType);
    _oauthRequest->setConsumerKey(APP_OAUTH_KEY);
    _oauthRequest->setConsumerSecretKey(APP_OAUTH_SECRET);
    _oauthRequest->setToken(_oauthUserToken);
    _oauthRequest->setTokenSecret(_oauthUserSecret);

    QNetworkRequest networkRequest;
    // Set the request's URL to the OAuth request's endpoint.
    networkRequest.setUrl(_oauthRequest->requestEndpoint());
    QList<QByteArray> requestHeaders = _oauthRequest->requestParameters();
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

    if (_oauthRequest->httpMethod() != HttpGET)
    {
        QLOG_FATAL("GenThumbnailDownloader::getThumbnail - encountered POST HTTP method");
    }

    _reply = _networkAccessmanager->get(networkRequest);
    connect(_reply, SIGNAL(finished()), SLOT(handleDownloadFinished()));
    connect(_reply, SIGNAL(readyRead()), this, SLOT(handleDataReady()));
}

void GenThumbnailDownloader::handleDownloadFinished()
{
    QLOG_DEBUG("GenThumbnailDownloader::handleDownloadFinished - thread: " << QThread::currentThreadId());
    QByteArray data = _reply->readAll();
    QVariant statusCodeVariant = _reply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute);
    int statusCode = 0;
    if (!statusCodeVariant.isNull())
    {
        statusCode = statusCodeVariant.toInt();
    }

    if (_reply->error() == QNetworkReply::NoError)
    {
        _thumbnail.append(data);
        emit thumbnailReady(_thumbnail);
    }
    else
    {
       emit thumbnailError(_reply->error(), statusCode, data);
    }


    _thumbnail.clear();
    _reply->deleteLater();
    _reply = 0;
}

void GenThumbnailDownloader::handleDataReady()
{
    QLOG_DEBUG("GenThumbnailDownloader::handleDataReady - thread:" << QThread::currentThreadId());

    if (_reply->error()>0)
    {
        QLOG_DEBUG("Network error: " << QString::number(_reply->error()));
        return;
    }
    _thumbnail.append(_reply->readAll());
}

void GenThumbnailDownloader::handleModelChanged(GenAppModel &model)
{
    QLOG_DEBUG("GenThumbnailDownloader::handleModelChanged");
    _oauthUserToken = model.settingValue(SETTINGS_OAUTH_TOKEN).toString();
    _oauthUserSecret = model.settingValue(SETTINGS_OAUTH_TOKEN_SECRET).toString();
}

void GenThumbnailDownloader::cancelThumbnailRetrieval()
{
    QLOG_DEBUG("GenThumbnailDownloader::cancelThumbnailRetrieval");
    if (_reply)
    {
        _reply->abort();
    }
}

