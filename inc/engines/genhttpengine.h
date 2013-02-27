#ifndef HTTPENGINE_H_
#define HTTPENGINE_H_

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class GenOAuthRequest;

class GenHTTPEngine : public QObject
{
Q_OBJECT
public:
    /*!
     * Http response enum
     */
    enum Response
    {
        OK = 200, /*!< OK */
        NOT_MODIFIED = 304, /*!< Not modified */
        BAD_REQUEST = 400, /*!< Bad request */
        NOT_AUTHORIZED = 401, /*!< Not authorized */
        FORBIDDEN = 403, /*!< Forbidden */
        NOT_FOUND = 404, /*!< Not found */
        INTERNAL_SERVER_ERROR = 500, /*!< Internal server error */
        BAD_GATEWAY = 502, /*!< Bad gateway */
        SERVICE_UNAVAILABLE = 503, /*!< Service unavailable */
        UNKNOWN = 2723
    /*!< Unknown */
    };

    /*!
     * \brief Constructor
     * \param parent, a Qobject argument. The parent object.
     */
    GenHTTPEngine(QObject *parent);

    /*!
     * \brief Destructor
     */
    virtual ~GenHTTPEngine();

    void executeRequest(GenOAuthRequest *request);


    /*!
     * \brief Aborts all pending operations
     */
    void abortOperations();

    /*!
     * \brief Sets the proxy
     */
    void setProxy(QString proxyHost, int proxyPort = 80, 
        QString proxyUser = "",
        QString proxyPass = "");

protected slots:
    /*!
     * \brief Response receiver slot
     * \param reply, a QNetworkReply* argument. The reply.
     */
    void finished(QNetworkReply *reply);

signals:
    /*!
     * \brief Signal, emmitted on a successfull http response
     * \param response, an int argument. The response code.
     * \param data, a const QByteArray& argument. The response data.
     * \param url, a const QUrl& arg. The url of the response.
     */
    void responseReceived(int httpStatusCode, const QByteArray &data = QByteArray());

    void responseError(QNetworkReply::NetworkError networkError,
                       int httpStatusCode,
                       QString errorString,
                       const QByteArray &data = QByteArray());

private:
    GenOAuthRequest *_r;

    QNetworkAccessManager *_manager; /*!< Network access manager instance */

    QNetworkReply *_reply; /*!< Network reply */

    bool _requestPending; /*!< Flag indicating whether a request is pending */    
};

#endif /* HTTPENGINE_H_ */
