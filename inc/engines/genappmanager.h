#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <QObject>
#include <QVariant>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>
#ifdef Q_OS_SYMBIAN
#include <QValueSpaceSubscriber>
#endif // Q_OS_SYMBIAN

QTM_USE_NAMESPACE

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
    void handleUserTokenReceived(QString token, QString secret);
    void handleAccountRegistered();
    void handleAccountInfoReceived(GenAccountInfo accountInfo);
    bool handleAction(const ActionId actionId);

private slots:
    void driveDismount(GenWatchedDrive drive);
    void profileChanged(QSystemDeviceInfo::Profile profile);
    void batteryLevelChanged(int batteryLevel);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int level);
    void startOauthTokenRequest(QString userName, QString userPass);
    void startRegisterRequest(QString firstName,
                              QString lastName,
                              QString email,
                              QString password);
    void retrieveAccountInfo();
    void timeChanged();
    void responseReceived();
    void responseError(QString error);
    void requestStarted();
    void handleAboutToQuit();
    void handleVkbStateChanged();
#if defined(Q_OS_SYMBIAN) && defined(FEATURE_ADS)
    void onStartReloadAd();
    void onAdDataReady();
    void onAdDataCorrupted();
    void onNetworkError(const QString& networkError);
#endif // defined(Q_OS_SYMBIAN) && defined(FEATURE_ADS)

private:
    static GenApplicationManager *_instance; /*!< Pointer to ApplicationManager instance */
    GenMainWindow *_mainWindow; /*!< The application's main window */
    GenAppModel *_applicationModel; /*!< The application model */
    GenFileServerUtils *_fsUtils; /*!< File server utils. Used to monitor memory card removal */
    QSystemDeviceInfo *_deviceInfo; /*!< Monitors the active profile, battery status>*/
    QSystemNetworkInfo *_networkInfo;
    QSystemDeviceInfo::Profile _activeProfile;
#ifdef Q_OS_SYMBIAN
    QValueSpaceSubscriber *_vkbstate;
#ifdef FEATURE_ADS
    InnerActiveAdModule *_adModule;
    InnerActiveAdWidget *_adWidget;
#endif // FEATURE_ADS
#endif // Q_OS_SYMBIAN
    GenWSEngine *_wsEngine;
    bool _isForward;
    int _batteryLevel;
    int _signalStrength;
    QString _email;
    QString _password;
};

#endif /* APPLICATIONMANAGER_H_ */

