#include "genfileserversymimpl.h"
#include "genfileserverutils.h"
#include "genlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FileServerSymImpl::FileServerSymImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
GenFileServerSymImpl::GenFileServerSymImpl(GenFileServerUtils& parent) :
    CActive(CActive::EPriorityStandard), GenFileServerImpl(parent)
{
    CActiveScheduler::Add(this);
    TInt err = iFs.Connect();
    iInitialized = (KErrNone == err);
    QLOG_DEBUG("FileServerSymImpl - initialized = " << iInitialized);
}

// -----------------------------------------------------------------------------
// FileServerSymImpl::~FileServerSymImpl
// C++ destructor
// -----------------------------------------------------------------------------
//
GenFileServerSymImpl::~GenFileServerSymImpl()
{
    Cancel();
    iFs.Close();
}

// -----------------------------------------------------------------------------
// FileServerSymImpl::notifyDismount
// Requests notifications for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerSymImpl::notifyDismount(GenWatchedDrive drive)
{
    m_watchedDrive = drive;
    if (!iInitialized) {
        return;
    }

    //    iFs.NotifyDismount( driveNumber, iStatus );
    iFs.NotifyChange(ENotifyDisk, iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// FileServerSymImpl::cancelNotifyDismount
// Cancel notification request for drive dismount
// -----------------------------------------------------------------------------
//
void GenFileServerSymImpl::cancelNotifyDismount()
{
    QLOG_DEBUG("cancelNotifyDismount");
    Cancel();
}

// -----------------------------------------------------------------------------
// FileServerSymImpl::DoCancel
// Implements cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
void GenFileServerSymImpl::DoCancel()
{
    if (!iInitialized) {
        return;
    }
    iFs.NotifyChangeCancel();
}

// -----------------------------------------------------------------------------
// FileServerSymImpl::RunL()
// Handles an active object's request completion event.
// -----------------------------------------------------------------------------
//
void GenFileServerSymImpl::RunL()
{
    QLOG_DEBUG("FileServerSymImpl::RunL() - iStatus = " << iStatus.Int());
    if (KErrNone == iStatus.Int()) {

        TDriveNumber driveNumber(EDriveC);
        switch (m_watchedDrive) {
        case DriveC:
        {
            driveNumber = EDriveC;
            break;
        }

        case DriveD:
        {
            driveNumber = EDriveD;
            break;
        }

        case DriveE:
        {
            driveNumber = EDriveE;
            break;
        }

        case DriveF:
        {
            driveNumber = EDriveF;
            break;
        }

        default:
        {
            driveNumber = EDriveC;
            break;
        }
        }
        TDriveInfo driveInfo;
        // Get the drive info for memory card 
        TInt err = iFs.Drive(driveInfo, driveNumber);
        if (err == KErrNone) {
            switch (driveInfo.iType)
            {
                case EMediaNotPresent:
                {
                    m_parent.DriveDismounted(m_watchedDrive);
                    break;
                }
                default:
                {
                    //MMC inserted          
                    break;
                }
            }
        }
    }
}
