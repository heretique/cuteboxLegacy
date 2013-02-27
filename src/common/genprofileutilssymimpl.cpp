#include "genprofileutilssymimpl.h"

#ifdef Q_OS_SYMBIAN

#include <profileenginesdkcrkeys.h>
#include <centralrepository.h>

GenProfileUtilsPrivate::GenProfileUtilsPrivate(GenProfileUtils *parent)
    : CActive(EPriorityStandard),
    q_ptr(parent), _initialized(false)
{
    CActiveScheduler::Add(this);
    
    iUid = KCRUidProfileEngine; //observe profile engine to
    iKey = KProEngActiveProfile; //see which profile is active
    iRepository = CRepository::NewL( iUid );
}

GenProfileUtilsPrivate::~GenProfileUtilsPrivate()
{
    Cancel();
    delete iRepository;
    iRepository=NULL;
}

GenProfileUtils::Profile GenProfileUtilsPrivate::profile() const
{
    return GenProfileUtils::Silent;
}

void GenProfileUtilsPrivate::RunL()
{
    TInt error = iRepository->Get( iKey, iActiveProfile);
    //always check the error code
    if( error == KErrNone )
    {
        //observer received the profile value as integer
        //this happens even if the value hasn’t really changed
        //some application may have set the same value again
        Q_Q(GenProfileUtils);
        q->profileChangedCallback(iActiveProfile);
    }
    // Re-subscribe
    error = iRepository->NotifyRequest( iKey, iStatus );
    if( error == KErrNone )
    {
        //
        SetActive();
    }
    else
    {
        //todo: error handling
    }
}

void GenProfileUtilsPrivate::DoCancel()
{
    iRepository->NotifyCancel(iKey);    
}

void GenProfileUtilsPrivate::monitor()
{
    if (!_initialized)
    {
        iRepository->Get( iKey, iActiveProfile);
        Q_Q(GenProfileUtils);
        q->profileChangedCallback(iActiveProfile);
        iRepository->NotifyRequest( iKey, iStatus );
        SetActive(); 
        _initialized = true;
    }
}

void GenProfileUtilsPrivate::stopmonitor()
{
    Cancel();
    _initialized = false;
}

#endif
