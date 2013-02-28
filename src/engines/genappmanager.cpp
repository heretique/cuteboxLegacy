#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>

#ifdef Q_OS_SYMBIAN
#include <coemain.h>    // for CCoeEnv, LIBS += -lcone
#include <w32std.h>     // for OrdinalPosition(), LIBS += -lws32
#include <eikenv.h>
#include <eikappui.h>
#include <aknappui.h>
#ifdef FEATURE_ADS
#include "InnerActiveAdModule.h"
#include "InnerActiveAdWidget.h"
#endif // FEATURE_ADS
#endif // Q_OS_SYMBIAN
#include "genfileserverutils.h"

#include "genappmanager.h"
#include "genmainwindow.h"
#include "genbaseview.h"
#include "genwelcomeview.h"
//#include "genloginview.h"
#include "genwebview.h"
#include "genmainview.h"
#include "gensettingsview.h"
#include "genregisterview.h"
#include "genfiledetailsview.h"
#include "genfilebrowserview.h"
#include "genlog.h"
#include "genappmodel.h"
#include "genresourcereader.h"
#include "genpublicstrings.h"
#include "genliterals.h"
#include "genwsengine.h"
#include "gendropboxrequests.h"
#include "genfileinfo.h"
#include "genconstants.h"
#include "gendataengine.h"
#include "genfileinfomodel.h"
#include "genstatuswidget.h"
#include "genindicatorswidget.h"
#ifdef FEATURE_PHOTO_WALL
#include "genphotoview.h"
#endif // FEATURE_PHOTO_WALL
#include "genhelpview.h"
#include "genjson.h"
#include "genlog.h"



// Static members
GenApplicationManager *GenApplicationManager::_instance = 0;

GenApplicationManager::GenApplicationManager() :
    _mainWindow(NULL),
    _applicationModel(new GenAppModel(this)),
    _fsUtils(new GenFileServerUtils(this)),
#ifdef Q_OS_SYMBIAN
    _deviceInfo(new QSystemDeviceInfo(this)),
    _networkInfo(new QSystemNetworkInfo(this)),
    _activeProfile(QSystemDeviceInfo::UnknownProfile),
    _vkbstate(new QValueSpaceSubscriber( "/vkb/state", this )),
#endif // Q_OS_SYMBIAN
    _wsEngine(new GenWSEngine(this))
{
#ifdef Q_OS_SYMBIAN
    // connect virtual keyboard state changes
    connect(_vkbstate, SIGNAL(contentsChanged()), SLOT(handleVkbStateChanged()));
#endif
    // Register for drive unmount notifications
    // This is needed when device is connected in Mass storage mode
    connect(_fsUtils, SIGNAL(sigDriveDismounted(GenWatchedDrive)), this,
            SLOT(driveDismount(GenWatchedDrive)));

    // Find the install drive
    char drive = QApplication::applicationDirPath().toLower().trimmed().at(0).toLower().toAscii();
    GenWatchedDrive wDrv;
    switch (drive) {
    case 'c':
        wDrv = DriveC;
        break;
    case 'd':
        wDrv = DriveD;
        break;
    case 'e':
        wDrv = DriveE;
        break;
    case 'f':
        wDrv = DriveF;
        break;
    default:
        wDrv = DriveC;
        break;
    }
    _fsUtils->notifyDismount(wDrv);
}

GenApplicationManager::~GenApplicationManager()
{
    delete _mainWindow;
    _mainWindow = 0;
}

GenApplicationManager *GenApplicationManager::instance()
{
    if (_instance) {
        return _instance;
    }
    else {
        _instance = new GenApplicationManager();
        return _instance;
    }
}

void GenApplicationManager::testData()
{
    QList<GenFileInfo*> testData;
    for (int i = 0;  i < 100; ++i)
    {
        GenFileInfo *fi = new GenFileInfo;
        fi->_path = QString("filename%1").arg(i);
        testData.append(fi);
    }
    _applicationModel->setFileInfoList(testData);
}

void GenApplicationManager::handleCancelFileAction()
{
    _wsEngine->cancelDownload();
    _wsEngine->cancelUpload();
}

void GenApplicationManager::initialize()
{
#ifdef IMEI_BUILD
    QString myImei = _deviceInfo->imei();
    if (myImei.isEmpty() || myImei != "355944044366557")
    {
        exit(0);
        return;
    }
#endif //IMEI_BUILD
        GenPublicStrings::retranslateStrings();
    QString downloadPath;
    downloadPath = _applicationModel->settingValue(SETTINGS_DOWNLOAD_LOCATION).toString();
    if (downloadPath.isEmpty())
    {
#ifdef Q_OS_SYMBIAN
        downloadPath = "E:/cuteBox";
#else
        downloadPath = "D:/cuteBox";
#endif
        _applicationModel->setSettingValue(SETTINGS_DOWNLOAD_LOCATION, downloadPath);
    }
    _mainWindow = new GenMainWindow(*_applicationModel);
    connect(_mainWindow, SIGNAL(animationFinished()), this, SLOT(handleViewAnimationFinished()));
    connect(_mainWindow, SIGNAL(actionTriggered(const ActionId)),
            this, SLOT(handleAction(const ActionId)));
    connect(_wsEngine, SIGNAL(responseReceived()), SLOT(responseReceived()));
    connect(_wsEngine, SIGNAL(responseError(QString)), SLOT(responseError(QString)));
    connect(_wsEngine, SIGNAL(requestStarted()), SLOT(requestStarted()));
    connect(_wsEngine, SIGNAL(uploadStarted(const QString&)),
            _mainWindow, SLOT(showFileProgressPopup(const QString&)));
    connect(_wsEngine, SIGNAL(uploadFinished()),
            _mainWindow, SLOT(hideFileProgressPopup()));
    connect(_wsEngine, SIGNAL(downloadStarted(const QString&)),
            _mainWindow, SLOT(showFileProgressPopup(const QString&)));
    connect(_wsEngine, SIGNAL(downloadFinished()),
            _mainWindow, SLOT(hideFileProgressPopup()));
    connect(_wsEngine, SIGNAL(fileProgress(qint64,qint64)),
            _mainWindow, SLOT(fileProgress(qint64,qint64)));
    connect(_wsEngine, SIGNAL(totalProgress(int,int)),
            _mainWindow, SLOT(totalProgress(int,int)));
    connect(_applicationModel,
            SIGNAL(modelChanged(GenAppModel&)),
            _wsEngine,
            SLOT(handleModelChanged(GenAppModel&)));

    GenBaseView *welcomeView = new GenWelcomeView(_applicationModel, _mainWindow);
    _mainWindow->addView(welcomeView);
    GenBaseView *webView = new GenWebView(_applicationModel, _mainWindow);
    _mainWindow->addView(webView);
    GenBaseView *mainView = new GenMainView(_wsEngine, _applicationModel, _mainWindow);
    _mainWindow->addView(mainView);
    GenSettingsView *settingsView = new GenSettingsView(_applicationModel, _mainWindow);
    _mainWindow->addView(settingsView);
    GenRegisterView *registerView = new GenRegisterView(_applicationModel, _mainWindow);
    _mainWindow->addView(registerView);
    GenFileDetailsView *fileDetailsView = new GenFileDetailsView(_wsEngine, _applicationModel, _mainWindow);
    _mainWindow->addView(fileDetailsView);
    GenFileBrowserView *fileBrowserView = new GenFileBrowserView(_applicationModel, _mainWindow);
    _mainWindow->addView(fileBrowserView);
    GenHelpView *helpView = new GenHelpView(_applicationModel, _mainWindow);
    _mainWindow->addView(helpView);
#ifdef FEATURE_PHOTO_WALL
    GenPhotoView *photoView = new GenPhotoView(_wsEngine, _applicationModel, _mainWindow);
    _mainWindow->addView(photoView);
#endif // FEATURE_PHOTO_WALL

    connect(webView,
            SIGNAL(authorized(QString,QString)),
            SLOT(handleAuthorized(QString,QString)));
    connect(webView,
            SIGNAL(notAuthorized(QString,QString)),
            SLOT(handleNotAuthorized(QString,QString)));
    connect(registerView,
            SIGNAL(registerAccount(QString,QString,QString,QString)),
            SLOT(startRegisterRequest(QString,QString,QString,QString)));
    connect(settingsView,
            SIGNAL(getAccountInfo()),
            SLOT(retrieveAccountInfo()));
    connect(_wsEngine, SIGNAL(authTokensReceived(QString,QString)),
            SLOT(handleAuthTokensReceived(QString,QString)));
    connect(_wsEngine, SIGNAL(accessTokensReceived(QString, QString, QString)),
            SLOT(handleUserTokenReceived(QString, QString, QString)));
    connect(_wsEngine, SIGNAL(accountCreated()),
            SLOT(handleAccountRegistered()));
    connect(_wsEngine,
            SIGNAL(accountInfoReceived(GenAccountInfo)),
            SLOT(handleAccountInfoReceived(GenAccountInfo)));
    connect(_applicationModel,
            SIGNAL(modelChanged(GenAppModel&)),
            settingsView,
            SLOT(handleModelChanged(GenAppModel&)));

    QString token = _applicationModel->settingValue(SETTINGS_OAUTH_TOKEN).toString();
    QString secret = _applicationModel->settingValue(SETTINGS_OAUTH_TOKEN_SECRET).toString();

    if (token.isEmpty() || secret.isEmpty()) {
        _mainWindow->setDefaultView(welcomeView);
    }
    else {
        _mainWindow->setDefaultView(mainView);
        _wsEngine->setUserToken(token, secret);
    }

#ifdef Q_OS_SYMBIAN
    // set indicator and connect for notifications
    connect(_deviceInfo, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            SLOT(profileChanged(QSystemDeviceInfo::Profile)));
    connect(_deviceInfo, SIGNAL(batteryLevelChanged(int)),
            SLOT(batteryLevelChanged(int)));
    connect(_networkInfo, SIGNAL(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)),
            SLOT(networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int)));

    _activeProfile = _deviceInfo->currentProfile();
    _batteryLevel = _deviceInfo->batteryLevel();
    QSystemNetworkInfo::NetworkMode mode = _networkInfo->currentMode();
    if (QSystemNetworkInfo::GsmMode == mode ||
            QSystemNetworkInfo::CdmaMode == mode ||
            QSystemNetworkInfo::WcdmaMode == mode)
    {
        _signalStrength = _networkInfo->networkSignalStrength(mode);
    }
#endif

    _mainWindow->statusBar()->indicatorsWidget()->setBatteryLevel(_batteryLevel);
    _mainWindow->statusBar()->indicatorsWidget()->setSignalLevel(_signalStrength);

    // set time
    _mainWindow->statusBar()->indicatorsWidget()->setClock(QTime::currentTime());
    QTimer *clockperiodic = new QTimer(this);
    connect(clockperiodic, SIGNAL(timeout()), SLOT(timeChanged()));
    clockperiodic->start(10 * 1000);
#if defined(Q_OS_SYMBIAN) && defined(FEATURE_ADS)
    initAdEngine();
#endif
}

#if defined(Q_OS_SYMBIAN) && defined(FEATURE_ADS)
void GenApplicationManager::initAdEngine()
{
    ///////////////////////////////////////////////////////////////
    // Set InnerActive Protocol parameters
    QString strAppUid;
    QString strPo;
    QString strEid;
    QString strAge;
    QString strGender;
    QString strMobileNumber;
    QString strCategory;
    QString strKeywords;
    QString strLocation;
    QString strLocationGps;

    if(strAppUid.isEmpty())
        strAppUid.append(QLatin1String("GeneraSoftware_cuteBox_OVI"));
    if(strPo.isEmpty())
        strPo.append(QLatin1String("551"));

    ///////////////////////////////////////////////////////////////
    // create InnerActive Ad Module
    _adModule = new InnerActiveAdModule(this, strAppUid);
    _adModule->setAdProtocolParameter(EIaProtocolParams_DistributionChannelPortal, strPo);
    _adModule->setAdProtocolParameter(EIaProtocolParams_ExternalId, strEid);
    _adModule->setAdProtocolParameter(EIaProtocolParams_Age, strAge);
    _adModule->setAdProtocolParameter(EIaProtocolParams_Gender, strGender);
    _adModule->setAdProtocolParameter(EIaProtocolParams_MobileNumber, strMobileNumber);
    _adModule->setAdProtocolParameter(EIaProtocolParams_Category, strCategory);
    _adModule->setAdProtocolParameter(EIaProtocolParams_Keywords, strKeywords);
    _adModule->setAdProtocolParameter(EIaProtocolParams_LocationString, strLocation);
    _adModule->setAdProtocolParameter(EIaProtocolParams_LocationGps, strLocationGps);

    connect(_adModule, SIGNAL(startReloadAd()), this, SLOT(onStartReloadAd()));
    connect(_adModule, SIGNAL(adDataReady()), this, SLOT(onAdDataReady()));
    connect(_adModule, SIGNAL(adDataCorrupted()), this, SLOT(onAdDataCorrupted()));
    connect(_adModule, SIGNAL(networkError(const QString&)), this, SLOT(onNetworkError(const QString&)));


    _adWidget = _adModule->createAdBanner(_mainWindow->statusBar());
    _adWidget->resize(360, 40);
    _adWidget->setObjectName(QLatin1String("adCtrl"));
    _adWidget->setFocusPolicy(Qt::StrongFocus);

    int res = _adModule->requestAd();
    Q_UNUSED(res);
}
#endif
void GenApplicationManager::run(QSplashScreen &splash)
{
    _mainWindow->currentView()->activate();
#ifdef Q_OS_SYMBIAN
    _mainWindow->showFullScreen();
#else
    _mainWindow->show();
#endif
    splash.finish(_mainWindow);
}

GenAppModel& GenApplicationManager::model()
{
    return *_applicationModel;
}


GenBaseView * GenApplicationManager::view(QString className)
{
    return _mainWindow->view(className);
}


bool GenApplicationManager::handleAction(const ActionId actionId)
{
    if (_mainWindow->currentView()->handleAction(actionId))
        return true;

    switch(actionId)
    {
    case ActionDROPBOXFILES:
        _mainWindow->activateView("GenMainView", true,
                                  true, GenFromTop);
        break;
    case ActionCANCELCURRENTFILEACTION:
        handleCancelFileAction();
        break;
    case ActionEXIT:
        QApplication::exit(0);
        break;
    default:
        break;
    }

    return true;
}

void GenApplicationManager::handleViewAnimationFinished()
{

}

void GenApplicationManager::handleAuthorized(QString uid, QString authToken)
{
    QLOG_DEBUG("[GenApplicationManager::handleAuthorized()]");
    _applicationModel->setSettingValue(SETTING_UID, uid);
    _wsEngine->setUserToken(authToken, _authSecret);
    startOauthTokenRequest("", "");
}

void GenApplicationManager::handleNotAuthorized()
{
    QLOG_DEBUG("[GenApplicationManager::handleNotAuthorized()]");
    _applicationModel->setSettingValue(SETTINGS_OAUTH_TOKEN, "");
    _applicationModel->setSettingValue(SETTINGS_OAUTH_TOKEN_SECRET, "");
    _applicationModel->setSettingValue(SETTING_UID, "");
    _mainWindow->activateView("GenWelcomeView", QVariant(), true, GenFromBottom);

}

void GenApplicationManager::handleAuthTokensReceived(QString authToken, QString authSecret)
{
    QString authUrl = "https://www.dropbox.com/1/oauth/authorize?oauth_token=" + authToken + "&display=mobile" + "&oauth_callback=" + REDIRECT_URL;
    _authToken = authToken;
    _authSecret = authSecret;
    _mainWindow->showWaitingIndicator("Loading...");
    _mainWindow->setViewData("GenWebView", authUrl);
}

void GenApplicationManager::handleUserTokenReceived(QString authToken, QString authSecret, QString userId)
{
    _applicationModel->setSettingValue(SETTINGS_OAUTH_TOKEN, authToken);
    _applicationModel->setSettingValue(SETTINGS_OAUTH_TOKEN_SECRET, authSecret);
    _wsEngine->setUserToken(authToken, authSecret);
    if (_applicationModel->isFirstRun())
        _mainWindow->activateView("GenHelpView", QVariant(), true, GenFromTop);
    else
        _mainWindow->activateView("GenMainView");
}

void GenApplicationManager::handleAccountRegistered()
{
    startOauthTokenRequest(_email, _password);
}

void GenApplicationManager::handleAccountInfoReceived(GenAccountInfo accountInfo)
{
    _applicationModel->setAccountInfo(accountInfo);
}


/*!
 * \brief Slot triggered when the watched drive has been unmounted.
 *        Usually, this drive should be the install drive.
 */
void GenApplicationManager::driveDismount(GenWatchedDrive drive)
{
    Q_UNUSED(drive)
    QApplication::quit();
}

void GenApplicationManager::batteryLevelChanged(int batteryLevel)
{
    _batteryLevel = batteryLevel;
    _mainWindow->statusBar()->indicatorsWidget()->setBatteryLevel(batteryLevel);
}

#ifdef Q_OS_SYMBIAN
void GenApplicationManager::profileChanged(QSystemDeviceInfo::Profile profile)
{
    _activeProfile = profile;
}

void GenApplicationManager::networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int level)
{
    //    QLOG_DEBUG("GenApplicationManager::networkSignalStrengthChanged - mode: " << mode
    //               << ", level: " << level);
    if (QSystemNetworkInfo::GsmMode == mode ||
            QSystemNetworkInfo::CdmaMode == mode ||
            QSystemNetworkInfo::WcdmaMode == mode)
    {
        _signalStrength = level;
        _mainWindow->statusBar()->indicatorsWidget()->setSignalLevel(level);
    }
}
#endif

void GenApplicationManager::requestAuthTokens() {
    QList<QPair<QString, QString> > params;
    _wsEngine->setUserToken("", "");
    _wsEngine->startWSRequest(GetDropboxRequestById(WSReqToken), "", params, QByteArray());
}

void GenApplicationManager::startOauthTokenRequest(QString userName, QString userPass)
{
    QList<QPair<QString, QString> > params;
    _wsEngine->startWSRequest(GetDropboxRequestById(WSReqAccessToken), "", params, QByteArray());
}

void GenApplicationManager::startRegisterRequest(QString firstName,
                                                 QString lastName,
                                                 QString email,
                                                 QString password)
{
    _applicationModel->setSettingValue(SETTINGS_EMAIL, email);
    _email = email;
    _password = password;
    QList<QPair<QString, QString> > params;
    params.append(qMakePair<QString, QString> ("first_name", firstName));
    params.append(qMakePair<QString, QString> ("last_name", lastName));
    params.append(qMakePair<QString, QString> ("email", email));
    params.append(qMakePair<QString, QString> ("password", password));
    params.append(qMakePair<QString, QString> ("oauth_consumer_key", APP_OAUTH_KEY));
    _wsEngine->startWSRequest(GetDropboxRequestById(WSReqAccount), "", params, QByteArray());
}

void GenApplicationManager::retrieveAccountInfo()
{
    _wsEngine->startWSRequest(GetDropboxRequestById(WSReqAccountInfo), QByteArray());
}



void GenApplicationManager::timeChanged()
{
    if (_mainWindow && _mainWindow->statusBar() && _mainWindow->statusBar()->indicatorsWidget())
        _mainWindow->statusBar()->indicatorsWidget()->setClock(QTime::currentTime());
}

bool GenApplicationManager::isBackground()
{
    QLOG_DEBUG("ApplicationManager::isBackground()");
    bool isBackground = false;
#ifdef Q_OS_SYMBIAN
    RWindowGroup* wg = &CCoeEnv::Static()->RootWin();
    if (wg->OrdinalPosition() != 0) {
        isBackground = true;
    }
#endif
    return isBackground;
}

void GenApplicationManager::responseReceived()
{
    _mainWindow->hideWaitingIndicator();
}

void GenApplicationManager::responseError(QString error)
{
    _mainWindow->hideWaitingIndicator();
    _mainWindow->showNotificationInfo(error);
}

void GenApplicationManager::requestStarted()
{
    _mainWindow->showWaitingIndicator("Loading...");
}

void GenApplicationManager::handleAboutToQuit()
{
    QLOG_DEBUG("GenApplicationManager::handleAboutToQuit");
    if (_applicationModel->isFirstRun())
        _applicationModel->setSettingValue(SETTING_FIRST_RUN, false);
}

void GenApplicationManager::handleVkbStateChanged()
{
#ifdef Q_OS_SYMBIAN
    bool ok = false;
    int val = _vkbstate->value().toInt( &ok );

    if (ok)
    {
        // ignore state changes if we're not the foreground application
        if ( isBackground() )
            return;
        CAknAppUi *appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
        if (appUi)
        {
            if (val)
            {
                    // Lock application orientation into Portrait
                    TRAP_IGNORE(appUi->SetOrientationL(CAknAppUi::EAppUiOrientationAutomatic));
            }
            else
            {
                TRAP_IGNORE(appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait));
            }
        }
    }
#endif // Q_OS_SYMBIAN
}

#if defined(Q_OS_SYMBIAN) && defined(FEATURE_ADS)

void GenApplicationManager::onStartReloadAd()
{
    QLOG_DEBUG("GenApplicationManager::onStartReloadAd");
}

void GenApplicationManager::onNetworkError(const QString& networkError)
{
    QLOG_DEBUG("GenApplicationManager::onNetworkError - error: " << networkError);
    QMessageBox::critical(_mainWindow, tr("Network Error"), networkError, QMessageBox::Ok);
}

void GenApplicationManager::onAdDataReady()
{
    QLOG_DEBUG("GenApplicationManager::onAdDataReady");
    int res = _adModule->updateBanner(_adWidget);
    bool bOk = false;
    switch (res)
    {
    case EIaErrors_AdDataNotReady:
        QMessageBox::information(_mainWindow, tr("InnerActive"), tr("Ad data not ready. Ad should be reloaded."));
        break;
    case EIaErrors_Processing:
        QMessageBox::information(_mainWindow, tr("InnerActive"), tr("Retrieving Ad data. Try again later."));
        break;
    case EIaErrors_Ok:
        {
        bOk = true;
        if(_adWidget->isHidden())
                _adWidget->show();
        }
        break;
    default:
        break;
    }
}

void GenApplicationManager::onAdDataCorrupted()
{
    QLOG_DEBUG("GenApplicationManager::onAdDataCorrupted");
}
#endif
