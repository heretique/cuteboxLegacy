#ifndef _GENFEEDBACKUTILSSYMIMPL_H_
#define _GENFEEDBACKUTILSSYMIMPL_H_

#include <touchlogicalfeedback.h>
#include "genfeedbackutils.h"

class MTouchFeedback;

class GenFeedbackUtilsPrivate
{
    Q_DECLARE_PUBLIC(GenFeedbackUtils)
public:
    GenFeedbackUtilsPrivate(GenFeedbackUtils *parent);
    ~GenFeedbackUtilsPrivate();

    void instantFeedback(GenFeedbackUtils::FeedBackType feedback);
    
public:
    GenFeedbackUtils * const q_ptr;
private:
    TTouchLogicalFeedback convertToSymbian(GenFeedbackUtils::FeedBackType effect);
    MTouchFeedback *_feedbackInstance;
};

#endif // _GENFEEDBACKUTILSSYMIMPL_H_
