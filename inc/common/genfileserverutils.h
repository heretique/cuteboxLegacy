#ifndef FILESERVERUTILS_H_
#define FILESERVERUTILS_H_

#include <QObject>
#include "genenums.h"

// FORWARD DECLARATIONS
class GenFileServerImpl;

class GenFileServerUtils : public QObject
    {
    Q_OBJECT
    
    friend class GenFileServerSymImpl;
    
public: // Constructors and destructor

    /**
     * C++ constructor.
     * @param parent parent of this object
     */
    GenFileServerUtils( QObject* parent = NULL );
    
    /**
     * Destructor.
     */
    ~GenFileServerUtils();
    
public: //New methods
    
    /**
     * Requests notifications for drive dismount
     * @param drive, drive to watch
     */
    void notifyDismount( GenWatchedDrive drive );
    
    /**
     * Cancel notification request for drive dismount
     */
    void cancelNotifyDismount();
    
private: //New methods
    
    /**
     * Watched drive has been dismounted
     * @param aDrive dismounted drive
     */
    void DriveDismounted( GenWatchedDrive aDrive );
    
signals: // New signals

    /**
     * Signal emited when a watched drive has been dismounted
     * @param dismounted drive
     */
    void sigDriveDismounted( GenWatchedDrive watchedDrive );
    
private: // Member data
    
    /**
     * File server implementation
     * Owned
     */
    GenFileServerImpl*     m_fileServer;
    };

#endif /* FILESERVERUTILS_H_ */
