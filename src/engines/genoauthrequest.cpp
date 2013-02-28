#include <QByteArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <QPair>
#include <QStringList>

#include <QtDebug>
#include <QtAlgorithms>
#include <QUuid>

#include "genoauthrequest.h"
#include "genoauthutils.h"
#include "genoauthcommon.h"
#include "genlog.h"


// This method will not include the "oauthSignature" paramater, since it is calculated from these parameters.
void GenOAuthRequest::prepareRequest() {

    // If parameter list is not empty, we don't want to insert these values by
    // accident a second time. So giving up.
    if( !_requestParameters.isEmpty() ) {
        return;
    }


    if (_requestType == AuthUser)
    {
        _requestParameters.append( qMakePair( OAUTH_KEY_SIGNATURE_METHOD, _oauthSignatureMethod ));
        _requestParameters.append( qMakePair( OAUTH_KEY_CONSUMER_KEY, _oauthConsumerKey ));
        _requestParameters.append( qMakePair( OAUTH_KEY_VERSION, _oauthVersion ));
        _requestParameters.append( qMakePair( OAUTH_KEY_TIMESTAMP, oauthTimestamp() ));
        _requestParameters.append( qMakePair( OAUTH_KEY_NONCE, oauthNonce() ));
        _requestParameters.append( qMakePair( OAUTH_KEY_TOKEN, _oauthToken ));
    } else if (_requestType == AuthApp) {
        _requestParameters.append( qMakePair( OAUTH_KEY_SIGNATURE_METHOD, _oauthSignatureMethod ));
        _requestParameters.append( qMakePair( OAUTH_KEY_CONSUMER_KEY, _oauthConsumerKey ));
        _requestParameters.append( qMakePair( OAUTH_KEY_VERSION, _oauthVersion ));
        _requestParameters.append( qMakePair( OAUTH_KEY_TIMESTAMP, oauthTimestamp() ));
        _requestParameters.append( qMakePair( OAUTH_KEY_NONCE, oauthNonce() ));
    }
    insertAdditionalParams(_requestParameters);
}

void GenOAuthRequest::insertAdditionalParams(QList< QPair<QString, QString> > &requestParams) {
    QList<QString> additionalKeys = _additionalParams.keys();
    QList<QString> additionalValues = _additionalParams.values();

    for(int i=0; i<additionalKeys.size(); i++) {
        requestParams.append( qMakePair(QString(additionalKeys.at(i)),
                                        QString(additionalValues.at(i)))
                              );
    }
}

void GenOAuthRequest::insertPostBody(const QByteArray &body)
{
    _postBodyContent.clear();
    _postBodyContent.append(body);
}

void GenOAuthRequest::signRequest() {
    QString signature = oauthSignature();
    _requestParameters.append( qMakePair( OAUTH_KEY_SIGNATURE, signature) );
}

QString GenOAuthRequest::oauthSignature()  {
    /**
     * http://oauth.net/core/1.0/#anchor16
     * The HMAC-SHA1 signature method uses the HMAC-SHA1 signature algorithm as defined in [RFC2104] where the 
     * Signature Base String is the text and the key is the concatenated values (each first encoded per Parameter 
     * Encoding) of the Consumer Secret and Token Secret, separated by an ‘&’ character (ASCII code 38) even if empty.
     **/
    QString baseString = requestBaseString();

    QString secret = QString(QUrl::toPercentEncoding(_oauthConsumerSecretKey)) + "&" + QString(QUrl::toPercentEncoding(_oauthTokenSecret));
    QString signature = GenOAuthUtils::hmac_sha1(baseString, secret);

    QLOG_DEBUG("========== GenOAuthRequest has the following signature:");
    QLOG_DEBUG(" * Signature : " << QUrl::toPercentEncoding(signature));

    return QString( QUrl::toPercentEncoding(signature) );
}

bool normalizedParameterSort(const QPair<QString, QString> &left, const QPair<QString, QString> &right) {
    QString keyLeft = left.first;
    QString valueLeft = left.second;
    QString keyRight = right.first;
    QString valueRight = right.second;

    if(keyLeft == keyRight) {
        return (valueLeft < valueRight);
    } else {
        return (keyLeft < keyRight);
    }
}
QByteArray GenOAuthRequest::requestBaseString() {
    QByteArray baseString;

    // Every request has these as the commont parameters.
    baseString.append( _oauthHttpMethodString.toUtf8() + "&");
    baseString.append( QUrl::toPercentEncoding(_oauthRequestEndpoint.toString(QUrl::RemovePath)));
    baseString.append( _oauthRequestEndpoint.path().toUtf8().toPercentEncoding("/", "~").toPercentEncoding()  + "&" ); // The path and query components

    QList< QPair<QString, QString> > baseStringParameters;
    baseStringParameters.append(_requestParameters);

    // Sort the request parameters. These parameters have been
    // initialized earlier.
    qSort(baseStringParameters.begin(),
          baseStringParameters.end(),
          normalizedParameterSort
          );

    // Last append the request parameters correctly encoded.
    baseString.append( encodedParamaterList(baseStringParameters) );


    QLOG_DEBUG("========== GenOAuthRequest has the following base string:");
    QLOG_DEBUG(baseString);

    return baseString;
}

QByteArray GenOAuthRequest::encodedParamaterList(const QList< QPair<QString, QString> > &parameters) {
    QByteArray resultList;

    bool first = true;
    QPair<QString, QString> parameter;

    QLOG_DEBUG("========== GenOAuthRequest has the following parameters:");

    foreach (parameter, parameters) {
        if(!first) {
            resultList.append( "&" );
        } else {
            first = false;
        }

        // Here we don't need to explicitely encode the strings to UTF-8 since
        // QUrl::toPercentEncoding() takes care of that for us.
        resultList.append( QUrl::toPercentEncoding(parameter.first)     // Parameter key
                           + "="
                           + QUrl::toPercentEncoding(parameter.second)  // Parameter value
                           );
        QLOG_DEBUG(" * " << parameter.first << " : " << parameter.second);

    }
    return QUrl::toPercentEncoding(resultList);
}

QString GenOAuthRequest::oauthTimestamp() const {
    // This is basically for unit tests only. In most cases we don't set the nonce beforehand.
    if (!_oauthTimestamp_.isEmpty()) {
        return _oauthTimestamp_;
    }

#if QT_VERSION >= 0x040700
    return QString::number(QDateTime::currentDateTimeUtc().toTime_t());
#else
    return QString::number(QDateTime::currentDateTime().toUTC().toTime_t());
#endif

}

QString GenOAuthRequest::oauthNonce() const {
    // This is basically for unit tests only. In most cases we don't set the nonce beforehand.
    if (!_oauthNonce_.isEmpty()) {
        return _oauthNonce_;
    }

    return QUuid::createUuid().toString() + QTime::currentTime().toString();
}

bool GenOAuthRequest::validateRequest() const
{
    if (_requestType == AuthUser &&
        (_oauthRequestEndpoint.isEmpty()
         || _oauthConsumerKey.isEmpty()
         || _oauthNonce_.isEmpty()
         || _oauthSignatureMethod.isEmpty()
         || _oauthTimestamp_.isEmpty()
         || _oauthToken.isEmpty()
         || _oauthTokenSecret.isEmpty()
         || _oauthVersion.isEmpty()))
    {
        return false;
    }
    return true;
}

//////////// Public implementation ////////////////

GenOAuthRequest::GenOAuthRequest(QObject *parent) :
        QObject(parent)
{
    qsrand(QTime::currentTime().msec());  // We need to seed the nonce random number with something.
    // However, we cannot do this while generating the nonce since
    // we might get the same seed. So initializing here should be fine.
}

GenOAuthRequest::~GenOAuthRequest()
{
    
}

void GenOAuthRequest::initRequest(GenWSAuthenticationType type, const QUrl &requestEndpoint) {

    if (!requestEndpoint.isValid()) {
        qWarning() << "Endpoint URL is not valid. Ignoring. This request might not work.";
        return;
    }

    if (type < 0 || type > AuthUser) {
        qWarning() << "Invalid request type. Ignoring. This request might not work.";
        return;
    }

    // Clear the request
    clearRequest();

    // Set smart defaults.
    _requestType = type;
    _oauthRequestEndpoint = requestEndpoint;
    _oauthTimestamp_ = oauthTimestamp();
    _oauthNonce_ = oauthNonce();
    this->setSignatureMethod(GenOAuthRequest::HMAC_SHA1);
    this->setHttpMethod(HttpPOST);
    _oauthVersion = "1.0"; // Currently supports only version 1.0

}

void GenOAuthRequest::setConsumerKey(const QString &consumerKey) {
    _oauthConsumerKey = consumerKey;
}

void GenOAuthRequest::setConsumerSecretKey(const QString &consumerSecretKey) {
    _oauthConsumerSecretKey = consumerSecretKey;
}

void GenOAuthRequest::setSignatureMethod(GenOAuthRequest::RequestSignatureMethod requestMethod) {
    QString requestMethodString;

    switch (requestMethod) {
    case GenOAuthRequest::PLAINTEXT:
        requestMethodString = "PLAINTEXT";
        break;
    case GenOAuthRequest::HMAC_SHA1:
        requestMethodString = "HMAC-SHA1";
        break;
    case GenOAuthRequest::RSA_SHA1:
        requestMethodString = "RSA-SHA1";
        break;
    default:
        // We should not come here
        qWarning() << "Invalid signature method set.";
        break;
    }

    _oauthSignatureMethod = requestMethodString;
}

void GenOAuthRequest::setTokenSecret(const QString &tokenSecret) {
    _oauthTokenSecret = tokenSecret;
}

void GenOAuthRequest::setToken(const QString &token) {
    _oauthToken = token;
}

void GenOAuthRequest::setHttpMethod(GenHTTPRequestType httpMethod) {
    QString requestHttpMethodString;

    switch (httpMethod) {
    case HttpGET:
        requestHttpMethodString = "GET";
        break;
    case HttpPOST:
        requestHttpMethodString = "POST";
        break;
    default:
        qWarning() << "Invalid HTTP method set.";
        break;
    }

    _oauthHttpMethod = httpMethod;
    _oauthHttpMethodString = requestHttpMethodString;
}

GenHTTPRequestType GenOAuthRequest::httpMethod() const {
    return _oauthHttpMethod;
}

void GenOAuthRequest::setAdditionalParameters(const GenOAuthParameters &additionalParams) {
    _additionalParams = additionalParams;
}

void GenOAuthRequest::setAdditionalParameters(const QList<QPair<QString, QString> > &additionalParams)
{
    _additionalParams.clear();
    for (int i = 0;  i < additionalParams.size(); ++i)
    {
        _additionalParams.insertMulti(additionalParams.at(i).first,
                                      additionalParams.at(i).second);
    }
}

GenOAuthParameters GenOAuthRequest::additionalParameters() const {
    return _additionalParams;
}

GenWSAuthenticationType GenOAuthRequest::requestType() const {
    return _requestType;
}

QUrl GenOAuthRequest::requestEndpoint() const {
    return _oauthRequestEndpoint;
}

QList<QByteArray> GenOAuthRequest::requestParameters() {
    QList<QByteArray> requestParamList;

    prepareRequest();
    if (!isValid() ) {
        qWarning() << "Request is not valid! I will still sign it, but it will probably not work.";
    }
    signRequest();

    QPair<QString, QString> requestParam;
    QString param;
    QString value;
    foreach (requestParam, _requestParameters) {
        param = requestParam.first;
        value = requestParam.second;
        requestParamList.append(QString(param + "=\"" + value +"\"").toUtf8());
    }

    return requestParamList;
}

QByteArray GenOAuthRequest::requestBody() const {
    return _postBodyContent;
}

bool GenOAuthRequest::isValid() const {
    return validateRequest();
}

void GenOAuthRequest::clearRequest() {
    _oauthRequestEndpoint = "";
    _oauthHttpMethodString = "";
    _oauthConsumerKey = "";
    _oauthConsumerSecretKey = "";
    _oauthToken = "";
    _oauthTokenSecret = "";
    _oauthSignatureMethod = "";
    _oauthCallbackUrl = "";
    _oauthTimestamp_ = "";
    _oauthNonce_ = "";
    _requestParameters.clear();
    _additionalParams.clear();
}
