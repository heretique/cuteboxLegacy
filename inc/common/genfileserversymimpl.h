#ifndef FILESERVERSYMIMPL_H_
#define FILESERVERSYMIMPL_H_

#include <e32base.h>
#include <f32file.h>

#include "genfileserverimpl.h"

class GenFileServerSymImpl : public CActive, public GenFileServerImpl
    {
public: // Constructors and destructor

    /**
     * C++ constructor.
     * @param parent, parent to be notified of file server events
     */
    GenFileServerSymImpl( GenFileServerUtils& parent );
    
    /**
     * Destructor.
     */
    ~GenFileServerSymImpl();
    
public: // From FileServerImpl
    
    /**
     * Requests notifications for drive dismount
     * @param drive, drive to watch
     */
    void notifyDismount( GenWatchedDrive drive );
    
    /**
     * Cancel notification request for drive dismount
     */
    void cancelNotifyDismount();
    
protected: // from CActive

    /**
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();
    
    /**
     * Handles an active object's request completion event.
     */
    void RunL();
    
protected: //Member data
    
    /**
     * File server session
     */
    RFs     iFs;
    
    /**
     * Flag indicating if the file server seesion has been initialized
     */
    TBool   iInitialized;
    };

#endif /* FILESERVERSYMIMPL_H_ */
