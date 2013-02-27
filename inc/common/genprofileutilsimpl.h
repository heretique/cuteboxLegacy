#ifndef PROFILEUTILSIMPL_H
#define PROFILEUTILSIMPL_H

#include "genprofileutils.h"

class GenProfileUtilsPrivate
{
    Q_DECLARE_PUBLIC(GenProfileUtils)
public:
    GenProfileUtilsPrivate(GenProfileUtils *parent);
    ~GenProfileUtilsPrivate();
    GenProfileUtils::Profile profile() const;
    void monitor();
    void stopmonitor();

protected:
    GenProfileUtils * const q_ptr;
};

#endif // PROFILEUTILSIMPL_H
