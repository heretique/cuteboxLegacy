#include <QTime>
#include <QStringList>
#include <QUrl>
#include <QVariant>
#include <QMap>

#include "genwsengine.h"
#include "genhttpengine.h"
#include "genoauthrequest.h"
#include "genjson.h"
#include "genliterals.h"
#include "gendropboxrequests.h"
#include "genfileinfo.h"
#include "genresourcereader.h"
#include "genfileuploader.h"
#include "genfiledownloader.h"
#include "genutils.h"
#include "genlog.h"

GenWSEngine::GenWSEngine(QObject *parent) :
    QObject(parent), _httpEngine(new GenHTTPEngine(this)),
    _oauthRequest(new GenOAuthRequest(this)),
    _fileUploader(new GenFileUploader(this)),
    _fileDownloader(new GenFileDownloader(this)),
    _currentWSRequestId(0),
    _currentProgress(0),
    _totalProgress(0),
    _move(false)
{
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    qRegisterMetaType<GenAccountInfo>("GenAccountInfo");

    connect(_httpEngine,
            SIGNAL(responseReceived(int, const QByteArray&)),
            SLOT(handleResponseReceived(int, QByteArray)));
    connect(_httpEngine,
            SIGNAL(responseError(QNetworkReply::NetworkError,int,QString,QByteArray)),
            SLOT(handleResponseError(QNetworkReply::NetworkError,int,QString,QByteArray)));
    connect(_fileUploader, SIGNAL(uploadFinished(bool,bool, const QString&)),
            SLOT(handleUploadFinished(bool,bool,const QString&)), Qt::QueuedConnection);
    connect(_fileUploader, SIGNAL(uploadProgress(qint64,qint64)),
            SIGNAL(fileProgress(qint64,qint64)));
    connect(_fileUploader, SIGNAL(uploadStarted(const QString&)),
            SIGNAL(uploadStarted(const QString&)));
    connect(_fileDownloader, SIGNAL(downloadFinished(QNetworkReply::NetworkError, int, QByteArray)),
            SLOT(handleDownloadFinished(QNetworkReply::NetworkError, int, QByteArray)), Qt::QueuedConnection);
    connect(_fileDownloader, SIGNAL(downloadProgress(qint64,qint64)),
            SIGNAL(fileProgress(qint64,qint64)));
    connect(_fileDownloader, SIGNAL(downloadStarted(const QString&)),
            SIGNAL(downloadStarted(const QString&)));
}

GenWSEngine::~GenWSEngine()
{
    
}

void GenWSEngine::parseResponse(const QByteArray &data)
{
    QLOG_INFO(">> GenWSEngine::parseResponse()");
    if (WSReqToken == _currentWSRequestId || WSReqAccessToken == _currentWSRequestId) {
        parseTokens(_currentWSRequestId, data);
    } else {
        QString error;
        QVariant result = Json::parse(data, &error);
        if (result.isNull())
        {
            QLOG_ERROR( "Failed to parse: " << error);
            return;
        }
        else
        {
            switch (_currentWSRequestId)
            {
//            case WSReqToken:
//                handleTokenReceived(result);
//                break;
            case WSReqGetMetadata:
                handleMetaDataReceived(result);
                break;
            case WSReqFileCopy:
            case WSReqFileMove:
                handleFileCopiedOrMoved(result);
                break;
            case WSReqFileDelete:
                handleFileDeleted(result);
                break;
            case WSReqAccountInfo:
                handleAccountInfoReceived(result);
                break;
            case WSReqCreateFolder:
                handleFolderCreated(result);
                break;
            default:
                break;
            }
        }
    }
    QLOG_INFO("<< GenWSEngine::parseResponse()");
}

void GenWSEngine::abortOperations()
{
    QLOG_INFO(">> GenWSEngine::abortOperations");
    
    _httpEngine->abortOperations();
    
    QLOG_INFO("<< GenWSEngine::abortOperations");
}

void GenWSEngine::startWSRequest(const GenWSRequestInfo *request, const QByteArray &data)
{
    QList<QPair<QString, QString> > params;
    startWSRequest(request, "", params, data);
}

void GenWSEngine::startWSRequest(const GenWSRequestInfo *request,
                                 const QString &path,
                                 const QByteArray &data)
{
    QList<QPair<QString, QString> > params;
    startWSRequest(request, path, params, data);
}

void GenWSEngine::startWSRequest(const GenWSRequestInfo *request,
                                 const QString &path,
                                 const QList<QPair<QString, QString> > &params,
                                 const QByteArray &data)
{
    _currentWSRequestId = request->_id;
    QString urlBase;
    if (request->_isSecure) {
        urlBase = "https://";
    }
    else {
        urlBase = "http://";
    }
    urlBase += request->_domain + DROPBOX_VERSION + request->_path;
    if (!path.isEmpty())
    {
        urlBase += path;
    }
    
    QUrl url = urlBase;
    _oauthRequest->initRequest(request->_authType, url);
    _oauthRequest->setHttpMethod(request->_httpType);
    _oauthRequest->setAdditionalParameters(params);
    _oauthRequest->setConsumerKey(APP_OAUTH_KEY);
    _oauthRequest->setConsumerSecretKey(APP_OAUTH_SECRET);
    _oauthRequest->setToken(_oauthUserToken);
    _oauthRequest->setTokenSecret(_oauthUserSecret);
    _oauthRequest->insertPostBody(data);
    _httpEngine->executeRequest(_oauthRequest);
    emit requestStarted();
}

void GenWSEngine::uploadFiles(const QString &path, FilesList files)
{
    _filesList = files;
    _destination = path;
    _currentProgress = 0;
    _totalProgress = _filesList.size();
    if (_totalProgress)
    {
        FilesList::const_iterator it = _filesList.constBegin();
        uploadFile(_destination, it.key());
    }
}

void GenWSEngine::uploadFile(const QString &path,
                             QString fileName)
{
    const GenWSRequestInfo *request = GetDropboxRequestById(WSReqFileUpload);
    _currentWSRequestId = request->_id;
    QString urlBase;
    if (request->_isSecure) {
        urlBase = "https://";
    }
    else {
        urlBase = "http://";
    }
    urlBase += request->_domain + DROPBOX_VERSION + request->_path;
    if (!path.isEmpty())
    {
        urlBase += path;
    }
    
    QUrl url = urlBase;
    _oauthRequest->initRequest(request->_authType, url);
    _oauthRequest->setHttpMethod(request->_httpType);
    QList<QPair<QString, QString> > params;
    params.append(qMakePair<QString, QString> ("file", fileName.mid(fileName.lastIndexOf("/") + 1).toUtf8()));
    _oauthRequest->setAdditionalParameters(params);
    _oauthRequest->setConsumerKey(APP_OAUTH_KEY);
    _oauthRequest->setConsumerSecretKey(APP_OAUTH_SECRET);
    _oauthRequest->setToken(_oauthUserToken);
    _oauthRequest->setTokenSecret(_oauthUserSecret);
    _fileUploader->upload(fileName.replace('\\', '/'), _oauthRequest);
}

void GenWSEngine::downloadFiles(FilesList files)
{
    _filesList = files;
    _currentProgress = 0;
    _totalProgress = _filesList.size();
    if (_totalProgress)
    {   FilesList::const_iterator it = files.constBegin();
        downloadFile(it.key(), it.value());
    }
}

void GenWSEngine::downloadFile(const QString &file, QDateTime lastModified)
{
    const GenWSRequestInfo *request = GetDropboxRequestById(WSReqFileDownload);
    _currentWSRequestId = request->_id;
    QString urlBase;
    if (request->_isSecure) {
        urlBase = "https://";
    }
    else {
        urlBase = "http://";
    }
    urlBase += request->_domain + DROPBOX_VERSION + request->_path;
    if (!file.isEmpty())
    {
        urlBase += file;
    }
    
    QUrl url = urlBase;
    _oauthRequest->initRequest(request->_authType, url);
    _oauthRequest->setHttpMethod(request->_httpType);
    _oauthRequest->setConsumerKey(APP_OAUTH_KEY);
    _oauthRequest->setConsumerSecretKey(APP_OAUTH_SECRET);
    _oauthRequest->setToken(_oauthUserToken);
    _oauthRequest->setTokenSecret(_oauthUserSecret);
    _fileDownloader->download(file, lastModified, _oauthRequest);
}

void GenWSEngine::copyOrMoveFiles(FilesList files, QString path, bool move)
{
    _filesList = files;
    _move = move;
    _destination = path;
    int pos = _destination.indexOf("/");
    if (pos != -1)
        _destination = _destination.mid(_destination.indexOf("/") + 1) + "/";
    else
        _destination = "";
    if (_filesList.size())
    {
        FilesList::const_iterator it = _filesList.constBegin();
        copyFile(it.key(), _destination, _move);
    }
}

void GenWSEngine::copyFile(QString file, QString destination, bool move)
{
    file = file.mid(file.indexOf("/") + 1);
    QList<QPair<QString, QString> > params;
    params.append(qMakePair<QString, QString> ("from_path", file));
    params.append(qMakePair<QString, QString> ("root", "dropbox"));
    QString target = destination;
    target += file.mid(file.lastIndexOf("/"));
    params.append(qMakePair<QString, QString> ("to_path", target));
    startWSRequest(GetDropboxRequestById(move ? WSReqFileMove: WSReqFileCopy), "", params, QByteArray());
}

void GenWSEngine::deleteFiles(FilesList files)
{
    _filesList = files;
    if (_filesList.size())
    {
        FilesList::const_iterator it = _filesList.constBegin();
        deleteFile(it.key());
    }
}

void GenWSEngine::deleteFile(QString file)
{
    file = file.mid(file.indexOf("/"));
    QList<QPair<QString, QString> > params;
    params.append(qMakePair<QString, QString> ("path", file));
    params.append(qMakePair<QString, QString> ("root", "dropbox"));
    startWSRequest(GetDropboxRequestById(WSReqFileDelete), "", params, QByteArray());
}

void GenWSEngine::createFolder(QString path)
{
    QList<QPair<QString, QString> > params;
    params.append(qMakePair<QString, QString> ("path", path));
    params.append(qMakePair<QString, QString> ("root", "dropbox"));
    startWSRequest(GetDropboxRequestById(WSReqCreateFolder), "", params, QByteArray());
}

void GenWSEngine::cancelDownload()
{
    _fileDownloader->cancelDownload();
    _filesList.clear();
}

void GenWSEngine::cancelUpload()
{
    _fileUploader->cancelUpload();
    _filesList.clear();
}


void GenWSEngine::setUserToken(const QString &token, const QString &secret)
{
    _oauthUserToken = token;
    _oauthUserSecret = secret;
}

const QString GenWSEngine::token() const
{
    return _oauthUserToken;
}

const QString GenWSEngine::tokenSecret() const
{
    return _oauthUserSecret;
}

void GenWSEngine::handleResponseReceived(int response, const QByteArray &data)
{
    emit responseReceived();
    QLOG_DEBUG("GenWSEngine::handleResponseReceived - responseCode :" << response
               << "\nDATA: " << data);
    if (GenHTTPEngine::OK == response) {
        parseResponse(data);
    }
    else {
        parseErrorResponse(response, data);
    }
}

void GenWSEngine::handleResponseError(QNetworkReply::NetworkError networkError,
                                      int httpStatusCode,
                                      QString errorString,
                                      const QByteArray &data)
{

    QString parseError = "Unknown Error";
    QString error;
    QVariant result;
    if (!data.isEmpty()) {
        result  = Json::parse(data, &parseError);
        if (result.isNull()) {
            QLOG_ERROR( "Failed to parse: " << error);
            return;
        }
    }
    if (result.isValid() && !result.isNull() && QVariant::Map == result.type())
        error = errorFromVariant(result.toMap());
    emit responseError(error);
    QLOG_DEBUG("GenWSEngine::handleResponseError - networkError: " << networkError
               << "\nHTTP STATUS: " << httpStatusCode
               << "\nerror String: " << errorString
               << "\nDATA: " << data);
}

GenHTTPEngine* GenWSEngine::httpEngine()
{
    return _httpEngine;
}

void GenWSEngine::parseErrorResponse(int response, const QByteArray &data)
{
    QLOG_DEBUG(">> GenWSEngine::parseErrorResponse");

    QString error;
    QVariant result;
    if (!data.isEmpty()) {
        result  = Json::parse(data, &error);
        if (result.isNull()) {
            QLOG_ERROR( "Failed to parse: " << error);
            return;
        }
    }
    switch(response)
    {
    case GenHTTPEngine::NOT_MODIFIED:
        emit metaDataNotModified();
        break;
    default:
        break;
    }
    QLOG_DEBUG("<< GenWSEngine::parseErrorResponse");
}

void GenWSEngine::parseTokens(int requestId, const QByteArray &data) {
    QString url = "http://test.com?" + data;
    QUrl parserUrl = url;
    QString secret = parserUrl.queryItemValue("oauth_token_secret");
    QString token = parserUrl.queryItemValue("oauth_token");
    if (requestId == WSReqAccessToken) {
        QString uid = parserUrl.queryItemValue("uid");
        emit accessTokensReceived(token, secret, uid);
    } else
        emit authTokensReceived(token, secret);
}

void GenWSEngine::handleUploadFinished(bool error, bool aborted, const QString &text)
{
    QLOG_DEBUG("GenWSEngine::handleUploadFinished - error = " << error
               << ", aborted = " << aborted
               << " description = " << text);
    if (_filesList.isEmpty())
    {
        emit uploadFinished();
        return;
    }
    _filesList.remove(_filesList.constBegin().key());
    emit totalProgress(++_currentProgress, _totalProgress);
    if (_filesList.size())
    {
        uploadFile(_destination, _filesList.constBegin().key());
    }
    else
        emit uploadFinished();
}

void GenWSEngine::handleDownloadFinished(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply)
{
    QLOG_DEBUG("GenWSEngine::handleDownloadFinished - error: " << error
               << ", HTTP Status: " << httpStatus
               << " reply: " << reply);
    if (_filesList.isEmpty())
    {
        emit downloadFinished();
        return;
    }
    _filesList.remove(_filesList.constBegin().key());
    emit totalProgress(++_currentProgress, _totalProgress);
    if (_filesList.size())
    {
        downloadFile(_filesList.constBegin().key(), _filesList.constBegin().value());
    }
    else
        emit downloadFinished();
}


void GenWSEngine::handleTokenReceived(const QVariant &variant)
{
    if (QVariant::Map != variant.type()) {
        QLOG_ERROR("GenWSEngine::handleTokenReceived - unknown response");
        return;
    }
    QVariantMap map = variant.toMap();
    QString token = map["token"].toString();
    QString secret = map["secret"].toString();
    if (token.isEmpty() || secret.isEmpty()) {
        QLOG_ERROR("GenWSEngine::handleTokenReceived - unknown response");
        return;
    }
//    emit accessTokensReceived(token, secret);
}

void GenWSEngine::handleMetaDataReceived(const QVariant &variant)
{
    if (QVariant::Map != variant.type()) {
        QLOG_ERROR("GenWSEngine::handleTokenReceived - unknown response");
        return;
    }
    QList<GenFileInfo*> content;
    QVariantMap map = variant.toMap();
    QString root = map["root"].toString();
    GenFileInfo *parent = convertFromVariant(root, map);
    QList<QVariant> contents = map["contents"].toList();
    for (int i = 0; i < contents.size(); ++i)
    {
        QVariantMap contentMap = contents.at(i).toMap();
        content.append(convertFromVariant(root, contentMap));
    }
    
    emit metaDataReceived(parent, content);
}

void GenWSEngine::handleAccountInfoReceived(const QVariant &variant)
{
    if (QVariant::Map != variant.type()) {
        QLOG_ERROR("GenWSEngine::handleAccountInfoReceived - unknown response");
        return;
    }
    QVariantMap map = variant.toMap();
    emit accountInfoReceived(accountInfoFromVariant(map));
}

void GenWSEngine::handleFileDeleted(const QVariant &variant)
{
    _filesList.remove(_filesList.constBegin().key());
    if (_filesList.size())
        deleteFile(_filesList.constBegin().key());
    else
        emit filesDeleted();
}

void GenWSEngine::handleFileCopiedOrMoved(const QVariant &variant)
{
    _filesList.remove(_filesList.constBegin().key());
    if (_filesList.size())
        copyFile(_filesList.constBegin().key(), _destination, _move);
    else
        emit filesCopiedOrMoved();
}

void GenWSEngine::handleFolderCreated(const QVariant &variant)
{
    if (QVariant::Map != variant.type()) {
        QLOG_ERROR("GenWSEngine::handleAccountInfoReceived - unknown response");
        return;
    }
    QVariantMap map = variant.toMap();
    QString root = map["root"].toString();
    if (!root.isEmpty())
    {
        emit folderCreated(convertFromVariant(root, map));
    }
}

GenFileInfo *GenWSEngine::convertFromVariant(const QString& root, const QVariantMap &variantMap)
{
    GenFileInfo *fileInfo = new GenFileInfo();
    QString boolean;
    
    fileInfo->_hash = variantMap["hash"].toString();
    fileInfo->_path = root + variantMap["path"].toString();
    fileInfo->_iconName = variantMap["icon"].toString();
    fileInfo->_size = variantMap["bytes"].toString().toInt();
    boolean = variantMap["thumb_exists"].toString();
    fileInfo->_hasThumb = convertToBool(boolean);
    boolean = variantMap["is_dir"].toString();
    fileInfo->_isDir = convertToBool(boolean);
    QString modifiedTime = variantMap["modified"].toString();
    fileInfo->_modifiedTime = convertToDateTime(modifiedTime);
    fileInfo->_icon = GenResourceReader::iconFromResource(fileInfo->_iconName);
    fileInfo->_thumbhash = thumbnailHash(fileInfo->_path);
    return fileInfo;
}

bool GenWSEngine::convertToBool(const QString &string)
{
    if ("false" == string)
        return false;
    
    return true;
}

QDateTime GenWSEngine::convertToDateTime(const QString &string)
{
    QDateTime dateTime = QLocale::c().toDateTime(string.left(string.length() - 6), TIME_FORMAT);
    dateTime.setTimeSpec(Qt::UTC);
    QLOG_DEBUG("GenWSEngine::convertToDateTime - input string: " << string <<", datetime: " << dateTime);
    return dateTime;
}

GenAccountInfo GenWSEngine::accountInfoFromVariant(const QVariantMap &variantMap)
{
    GenAccountInfo accountInfo;
    accountInfo._uid = variantMap["uid"].toString();
    accountInfo._country = variantMap["country"].toString();
    accountInfo._displayName = variantMap["display_name"].toString();
    QVariantMap quotaInfo = variantMap["quota_info"].toMap();
    if (!quotaInfo.isEmpty())
    {
        accountInfo._sharedSpace = quotaInfo["shared"].toULongLong();
        accountInfo._usedSpace = quotaInfo["normal"].toULongLong();
        accountInfo._spaceLimit = quotaInfo["quota"].toULongLong();
    }
    return accountInfo;
}

QString GenWSEngine::errorFromVariant(const QVariantMap &variantMap)
{
    if (QVariant::String == variantMap["error"].type())
        return variantMap["error"].toString();
    else if (QVariant::Map == variantMap["error"].type())
    {
        QVariantMap map = variantMap["error"].toMap();
        QVariantMap::const_iterator it = map.constBegin();
        QString error;
        while (it != map.constEnd())
        {
            error += it.value().toString();
            ++it;
        }
        return error;
    }
    return QString("");
}

void GenWSEngine::handleModelChanged(GenAppModel& model)
{
    _fileDownloader->handleModelChanged(model);
}
