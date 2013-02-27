#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDir>
#if defined(Q_OS_SYMBIAN)
#include <f32file.h>
#elif defined(Q_OS_WIN32)
#include <windows.h>
#endif

#include "genfiledownloader.h"
#include "genoauthrequest.h"
#include "genupfile.h"
#include "genappmodel.h"
#include "genliterals.h"
#include "genlog.h"

GenFileDownloader::GenFileDownloader(QObject *parent)
    : QObject(parent),
      _networkAccessmanager(new QNetworkAccessManager(this)),
      _reply(NULL),
      _downloadedFile(NULL),
      _isAborted(false),
      _isInProgress(false),
      #ifdef Q_OS_SYMBIAN
      _downloadPath("E:/cuteBox")
    #else
      _downloadPath("D:/cuteBox")
    #endif
{

}

GenFileDownloader::~GenFileDownloader()
{
    delete _reply;
}

void GenFileDownloader::download(const QString &fileName, QDateTime lastModified, GenOAuthRequest *request)
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

    _lastModified = lastModified;
    QNetworkRequest networkRequest;
    // Set the request's URL to the OAuth request's endpoint.
    networkRequest.setUrl(request->requestEndpoint());
    QLOG_DEBUG("GenFileDownloader::upload - url = " << request->requestEndpoint());
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

    if (request->httpMethod() != HttpGET)
    {
        QLOG_FATAL("GenFileDownloader::download - encountered POST HTTP method");
    }
    _isAborted = false;


    QString path;
    path = _downloadPath + "/";
    path += fileName;
    QDir dir(path.left(path.lastIndexOf("/")));
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    path += ".part";
    delete _downloadedFile;
    _downloadedFile = new QFile(path);
    if (_downloadedFile->open(QIODevice::WriteOnly))
    {
        _reply = _networkAccessmanager->get(networkRequest);
        connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));
        connect(_reply, SIGNAL(finished()), SLOT(handleDownloadFinished()));
        connect(_reply, SIGNAL(readyRead()), this, SLOT(handleDataReady()));

        _isInProgress = true;
        QString shortFilename = fileName.mid(fileName.lastIndexOf("/") + 1);
        emit downloadStarted(shortFilename);
    }
    else
    {
        _downloadedFile->close();
        _downloadedFile->remove();
        emit downloadFinished(QNetworkReply::NoError, 666, QByteArray());
        delete _downloadedFile;
        _downloadedFile = 0;
    }
}

void GenFileDownloader::setLastModified(QString fileName, QDateTime lastModified)
{
#if defined(Q_OS_SYMBIAN)
    RFs fs;
    fs.Connect();
    CleanupClosePushL(fs);
    RFile file;
    TPtrC16 nativePath(reinterpret_cast<const TUint16*>(QDir::toNativeSeparators(fileName).utf16()));
    if (KErrNone == file.Open(fs, nativePath, EFileShareAny | EFileWrite))
    {
        CleanupClosePushL(file);
        if (lastModified.isValid())
        {
            TTime time = TTime( TDateTime( 1970, EJanuary, 0, 0, 0, 0, 0 ) );
            time += TTimeIntervalSeconds(lastModified.toTime_t());
            file.SetModified(time);
        }
        CleanupStack::PopAndDestroy(&file);
    }
    
    CleanupStack::PopAndDestroy(&fs);
    
#elif defined(Q_OS_WIN32)
    SYSTEMTIME st;
    FILETIME ft;
    st.wDay = lastModified.date().day();
    st.wDayOfWeek = lastModified.date().dayOfWeek();
    st.wMonth = lastModified.date().month();
    st.wYear = lastModified.date().year();
    st.wHour = lastModified.time().hour();
    st.wMinute = lastModified.time().minute();
    st.wSecond = lastModified.time().second();
    st.wMilliseconds = lastModified.time().msec();
    SystemTimeToFileTime(&st, &ft);
    QString nativePath = QDir::toNativeSeparators(fileName);

    HANDLE fh = CreateFile((wchar_t*)nativePath.utf16(),
                           FILE_WRITE_ATTRIBUTES,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           0,
                           OPEN_EXISTING,
                           FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
                           0);
    if (fh != INVALID_HANDLE_VALUE)
    {
        BOOL ret = SetFileTime(fh,           // Sets last-write time of the file
                               (LPFILETIME) NULL,           // to the converted current system time
                               (LPFILETIME) NULL,
                               &ft);
        if (!ret)
        {
            DWORD errCode = GetLastError();
            QLOG_DEBUG("GenFileDownloader::setLastModified - SetFileTime error: " << errCode);
        }
    }
    CloseHandle(fh);
#else

#endif
#ifdef Q_LOGGING
    QFileInfo fi(fileName);
    QDateTime newLastModified = fi.lastModified().toUTC();
    QLOG_DEBUG("GenFileDownloader::setLastModified - remote: "
               << lastModified
               << ", local: "
               << newLastModified);
#endif
}

void GenFileDownloader::handleDownloadFinished()
{
    Q_ASSERT(_downloadedFile);
    _downloadedFile->close();
    _isInProgress = false;

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
        QString fn = _downloadedFile->fileName();
        fn = fn.mid(0, fn.size()-5);
        QFileInfo fileInfo(fn);
        if (fileInfo.exists())
        {
            QFile file;
            file.remove(fn);
        }
        _downloadedFile->rename(fn);
    }
    else
    {
        _downloadedFile->remove();
    }

    emit downloadFinished(_reply->error(), statusCode, data);

    QString fileName = _downloadedFile->fileName();
    delete _downloadedFile;
    _downloadedFile = 0;

    if (_reply->error() == QNetworkReply::NoError)
    {
        setLastModified(fileName, _lastModified);
    }

    _reply->deleteLater();
    _reply = 0;
}

void GenFileDownloader::handleDataReady()
{
    if (_reply->error()>0)
    {
        QLOG_DEBUG("Network error: " << QString::number(_reply->error()));
        return;
    }
    QByteArray bytes = _reply->readAll();
    if (_downloadedFile->write(bytes)<0)
    {
        QLOG_DEBUG("Error writing file: " << _downloadedFile->fileName());
        //filemutex.unlock();
        _reply->abort();
        _isAborted = true;
        return;
    }
}

void GenFileDownloader::handleModelChanged(GenAppModel &model)
{
    _downloadPath = model.settingValue(SETTINGS_DOWNLOAD_LOCATION).toString();
}

void GenFileDownloader::cancelDownload()
{
    if (_reply)
    {
        _reply->abort();
    }
}
