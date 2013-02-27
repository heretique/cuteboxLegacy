#ifndef PROFILEUTILSSYMIMPL_H
#define PROFILEUTILSSYMIMPL_H


#include "genprofileutils.h"
#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class CRepository; //forward declaration

class GenProfileUtilsPrivate : public CActive
{
    Q_DECLARE_PUBLIC(GenProfileUtils)
public:
    GenProfileUtilsPrivate(GenProfileUtils *parent);
    ~GenProfileUtilsPrivate();
    GenProfileUtils::Profile profile() const;
    void monitor();
    void stopmonitor();
    
protected: //from CActive
    void RunL();
    void DoCancel();
public:
    GenProfileUtils * const q_ptr;
private:
    CRepository* iRepository;
    TUint32 iKey;
    TUid iUid;
    TInt iActiveProfile;
    bool _initialized;
};

#endif // PROFILEUTILSSYMIMPL_H
