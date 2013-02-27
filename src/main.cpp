#include <QtGui/QApplication>
#include <QFileInfo>
#include <QPixmapCache>
#include <QSplashScreen>

#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknappui.h>
#endif

#include "genlog.h"
#include "genlogdest.h"
#include "genappmanager.h"
#include "genresourcereader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if defined(Q_LOGGING) || defined(Q_LOGGING_PROFILE)
    QLogging::Logger& logger = QLogging::Logger::instance();
    logger.setLoggingLevel(QLogging::LogALLLevel);
    const QString logPath("C:\\logs\\" + QFileInfo(
            QCoreApplication::applicationFilePath()).baseName() + ".log");
    QLogging::DestinationPtr fileDestination(
            QLogging::DestinationFactory::MakeFileDestination(logPath));
    QLogging::DestinationPtr debugDestination(
            QLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());
#endif
    // used for proper xml format
    QLOG_PROFILING("<profiling>");

#ifdef Q_OS_SYMBIAN
    CAknAppUi *appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    if (appUi)
    {
        // Lock application orientation into Portrait
        TRAP_IGNORE(appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait));
    }
#endif 
    
    a.setOrganizationName("Genera");
    a.setApplicationName("cuteBox");
#ifdef Q_OS_SYMBIAN
    QPixmapCache::setCacheLimit(5120);
#endif
    QPixmap pixmap(":/images/splashscreen.png");
    QSplashScreen splash(pixmap);
#ifdef Q_OS_SYMBIAN
    splash.showFullScreen();
    splash.raise();
#else
    splash.show();
#endif
//    GenResourceReader::setResourceFile(":/resources.svg");

    GenApplicationManager *appManager = GenApplicationManager::instance();
    appManager->initialize();
    appManager->run(splash);
    QObject::connect(&a, SIGNAL(aboutToQuit()), appManager, SLOT(handleAboutToQuit()));
    int errCode = a.exec();
    QLOG_PROFILING("</profiling>");
    delete appManager;

    return errCode;
}
