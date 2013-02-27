#ifndef _GENFEEDBACKUTILSIMPL_H_
#define _GENFEEDBACKUTILSIMPL_H_


#include "genfeedbackutils.h"

class GenFeedbackUtilsPrivate
{
    Q_DECLARE_PUBLIC(GenFeedbackUtils)
public:
    GenFeedbackUtilsPrivate(GenFeedbackUtils *parent);
    ~GenFeedbackUtilsPrivate();

    void instantFeedback(GenFeedbackUtils::FeedBackType feedback);
    
public:
    GenFeedbackUtils * const q_ptr;
};
#endif // _GENFEEDBACKUTILSIMPL_H_
