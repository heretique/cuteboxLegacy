#include "genfileserverimpl.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// FileServerImpl::FileServerImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
GenFileServerImpl::GenFileServerImpl( GenFileServerUtils& parent ) :
    m_parent( parent ), m_watchedDrive( DriveC )
    {
    // nothing to do
    }

// -----------------------------------------------------------------------------
// FileServerImpl::~FileServerImpl
// C++ destructor
// -----------------------------------------------------------------------------
//
GenFileServerImpl::~GenFileServerImpl()
    {
    // nothing to do
    }

// -----------------------------------------------------------------------------
// FileServerImpl::notifyDismount
// Requests notifications for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerImpl::notifyDismount( GenWatchedDrive drive )
    {
    m_watchedDrive = drive;
    }

// -----------------------------------------------------------------------------
// FileServerImpl::cancelNotifyDismount
// Cancel notification request for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerImpl::cancelNotifyDismount()
    {
    // nothing to do
    }
