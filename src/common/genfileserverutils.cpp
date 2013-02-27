#include <qglobal.h>

#include "genfileserverutils.h"

#ifdef Q_OS_SYMBIAN
#include "genfileserversymimpl.h"
#else
#include "genfileserverimpl.h"
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FileServerUtils::FileServerUtils
// C++ constructor
// -----------------------------------------------------------------------------
//
GenFileServerUtils::GenFileServerUtils( QObject* parent ) : QObject( parent ),
    m_fileServer( NULL )
    {
    #ifdef Q_OS_SYMBIAN
    m_fileServer = new GenFileServerSymImpl( *this );
    #else
    m_fileServer = new GenFileServerImpl( *this );
    #endif
    }

// -----------------------------------------------------------------------------
// FileServerUtils::~FileServerUtils
// C++ destructor
// -----------------------------------------------------------------------------
//
GenFileServerUtils::~GenFileServerUtils()
    {
    delete m_fileServer;
    }

// -----------------------------------------------------------------------------
// FileServerUtils::notifyDismount
// Requests notifications for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerUtils::notifyDismount( GenWatchedDrive drive )
    {
    m_fileServer->notifyDismount( drive );
    }

// -----------------------------------------------------------------------------
// FileServerUtils::cancelNotifyDismount
// Cancel notification request for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerUtils::cancelNotifyDismount()
    {
    m_fileServer->cancelNotifyDismount();
    }

// -----------------------------------------------------------------------------
// FileServerUtils::DriveDismounted
// Watched drive has been dismounted
// -----------------------------------------------------------------------------
//
void GenFileServerUtils::DriveDismounted( GenWatchedDrive aDrive )
    {
    emit sigDriveDismounted( aDrive );
    }
