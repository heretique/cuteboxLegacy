#ifndef FILESERVERIMPL_H_
#define FILESERVERIMPL_H_

#include "genenums.h"

// FORWARD DECLARATIONS
class GenFileServerUtils;

class GenFileServerImpl
    {
public: // Constructors and destructor

    /**
     * C++ constructor.
     * @param parent, parent to be notified of file server events
     */
    GenFileServerImpl( GenFileServerUtils& parent );
    
    /**
     * Destructor.
     */
    virtual ~GenFileServerImpl();
    
public: //New methods
    
    /**
     * Requests notifications for drive dismount
     * @param drive, drive to watch
     */
    virtual void notifyDismount( GenWatchedDrive drive );
    
    /**
     * Cancel notification request for drive dismount
     */
    virtual void cancelNotifyDismount();
    
protected: //Member data
    
    /**
     * Events observer
     */
    GenFileServerUtils&        m_parent;
    
    /**
     * Watched drive
     */
    GenWatchedDrive            m_watchedDrive;
    };

#endif /* FILESERVERIMPL_H_ */
