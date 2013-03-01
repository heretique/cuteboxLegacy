#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <QObject>
#include <QVariant>
#include <QUrl>
#ifdef Q_OS_SYMBIAN
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>
#include <QValueSpaceSubscriber>

QTM_USE_NAMESPACE

#endif // Q_OS_SYMBIAN


#include "genenums.h"
#include "genstateevent.h"
#include "genprofileutils.h"
#include "genfileserverutils.h"
#include "geninterfaces.h"


class GenMainWindow;
class GenAppModel;
class GenResourceReader;
class GenBaseView;
class GenWSEngine;
class GenDataEngine;
class GenFileInfo;
class GenMainView;
class QSplashScreen;
class GenPhotoView;
class InnerActiveAdModule;
class InnerActiveAdWidget;

class GenApplicationManager : public QObject, public GenControllerInterface
{
    Q_OBJECT
public:

    /*!
     * \brief Instance of ApplicationManager
     * \return ApplicationManager&
     */
    static GenApplicationManager* instance();

    /*!
     * \brief Destructor
     */
    ~GenApplicationManager();

    /*!
     * \brief Instantiates member objects
     */
    void initialize();

    void initAdEngine();

    /*!
     * \brief Starts the application manager
     */
    void run(QSplashScreen &splash);

    /*!
     * \brief Returns a reference to the application model
     * \return ApplicationModel&
     */
    GenAppModel &model();
    
    GenBaseView *view(QString className);

    void testData();
    void requestAuthTokens();

protected:
    void handleCancelFileAction();

private:
    /*!
     * \brief Consructor.
     */
    GenApplicationManager();
    
    bool isBackground();

protected slots:
    void handleViewAnimationFinished();
    void handleAuthorized(QString uid, QString authToken);
    void handleNotAuthorized();
    void handleAuthTokensReceived(QString authToken, QString authSecret);
    void handleUserTokenReceived(QString authToken, QString authSecret, QString userId);
    void handleAccountRegistered();
    void handleAccountInfoReceived(GenAccountInfo accountInfo);
    bool handleAction(const ActionId actionId);

private slots:
    void driveDismount(GenWatchedDrive drive);
    void batteryLevelChanged(int batteryLevel);
#ifdef Q_OS_SYMBIAN
    void profileChanged(QSystemDeviceInfo::Profile profile);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int level);
#endif
    void startOauthTokenRequest(QString userName, QString userPass);
    void retrieveAccountInfo();
    void timeChanged();
    void responseReceived();
    void responseError(QString error);
    void requestStarted();
    void handleAboutToQuit();
    void handleVkbStateChanged();
#if defined(FEATURE_ADS) && (defined(Q_OS_SYMBIAN) || defined(QT_SIMULATOR))
    void onStartReloadAd();
    void onAdDataReady();
    void onAdDataCorrupted();
    void onNetworkError(const QString& networkError);
#endif // defined(FEATURE_ADS)

private:
    static GenApplicationManager *_instance; /*!< Pointer to ApplicationManager instance */
    GenMainWindow *_mainWindow; /*!< The application's main window */
    GenAppModel *_applicationModel; /*!< The application model */
    GenFileServerUtils *_fsUtils; /*!< File server utils. Used to monitor memory card removal */

#ifdef Q_OS_SYMBIAN
    QSystemDeviceInfo *_deviceInfo; /*!< Monitors the active profile, battery status>*/
    QSystemNetworkInfo *_networkInfo;
    QSystemDeviceInfo::Profile _activeProfile;
    QValueSpaceSubscriber *_vkbstate;
#endif // Q_OS_SYMBIAN

#if defined(FEATURE_ADS) && (defined(Q_OS_SYMBIAN) || defined(QT_SIMULATOR))
    InnerActiveAdModule *_adModule;
    InnerActiveAdWidget *_adWidget;
#endif // FEATURE_ADS

    GenWSEngine *_wsEngine;
    bool _isForward;
    int _batteryLevel;
    int _signalStrength;
    QString _email;
    QString _password;
    QString _authToken;
    QString _authSecret;
};

#endif /* APPLICATIONMANAGER_H_ */

