#include <QNetworkProxy>
#include <QTimer>

#include "genhttpengine.h"
#include "genoauthrequest.h"
#include "genlog.h"

QNetworkProxy::ProxyType proxyType = QNetworkProxy::QNetworkProxy::HttpProxy;


QList<QPair<QString, QString> > createQueryParams(
    const GenOAuthParameters &requestParams)
{
    QList<QString> requestKeys = requestParams.keys();
    QList<QString> requestValues = requestParams.values();

    QList<QPair<QString, QString> > result;
    for (int i = 0; i < requestKeys.size(); i++) {
        result.append(qMakePair(requestKeys.at(i), requestValues.at(i)));
    }

    return result;
}

GenHTTPEngine::GenHTTPEngine(QObject *parent) :
        QObject(parent), _r(NULL),
        _manager(new QNetworkAccessManager(this)),
        _reply(NULL), _requestPending(false)

{
    setProxy("NO_PROXY");
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(finished(QNetworkReply*)));
}

GenHTTPEngine::~GenHTTPEngine()
{

}

void GenHTTPEngine::executeRequest(GenOAuthRequest *request)
{
    _r = request;

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

    if (request->httpMethod() == HttpGET)
    {
        // Get the requested additional params as a list of pairs we can give QUrl
        QList<QPair<QString, QString> > urlParams = createQueryParams(
                request->additionalParameters());

        // Take the original URL and append the query params to it.
        QUrl urlWithParams = networkRequest.url();
        urlWithParams.setQueryItems(urlParams);
        networkRequest.setUrl(urlWithParams);

        // Submit the request including the params.
        _reply = _manager->get(networkRequest);
    }
    else if (request->httpMethod() == HttpPOST)
    {
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                                 "application/x-www-form-urlencoded");
        QLOG_DEBUG("GenHTTPEngine::executeRequest - post body: " << request->requestBody());
        _reply = _manager->post(networkRequest, request->requestBody());
    }
}

void GenHTTPEngine::abortOperations()
{
    QLOG_INFO(">>HTTPEngine::abortOperations");

    if (_reply &&_reply->isOpen())
    {
        QLOG_INFO("HTTPEngine::abortOperations request aborted");
        _reply->abort();
        _reply = NULL;
    }

    QLOG_INFO("<<HTTPEngine::abortOperations");
}

void GenHTTPEngine::setProxy(QString proxyHost, int proxyPort, QString proxyUser,
                             QString proxyPass)
{
    QLOG_DEBUG("HTTPEngine::setProxy");

    if (!proxyHost.isEmpty() && proxyHost != "NO_PROXY")
    {
        _manager->setProxy(QNetworkProxy(proxyType, proxyHost, proxyPort,
                                         proxyUser, proxyPass));
    }
    else
    {
        _manager->setProxy(QNetworkProxy::NoProxy);
    }
}

void GenHTTPEngine::finished(QNetworkReply *reply)
{
    QLOG_DEBUG("HTTPEngine::finished" );

    QByteArray data = reply->readAll();
    QVariant statusCodeVariant = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute);
    int statusCode = UNKNOWN;
    if (!statusCodeVariant.isNull())
    {
        statusCode = statusCodeVariant.toInt();
    }
    if (reply->error() == QNetworkReply::NoError)
    {
        emit responseReceived(statusCode, data);
    }
    else
    {
        emit responseError(reply->error(), statusCode,
                           reply->errorString(), data);
    }

    _requestPending = false;
    _reply = NULL;

    reply->deleteLater();
}
