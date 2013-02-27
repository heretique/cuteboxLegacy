#include "genfeedbackutils.h"
#ifdef Q_OS_SYMBIAN
#include "genfeedbackutilssymimpl.h"
#else
#include "genfeedbackutilsimpl.h"
#endif


GenFeedbackUtils::GenFeedbackUtils() :
d_ptr(new GenFeedbackUtilsPrivate(this))
{

}

GenFeedbackUtils::~GenFeedbackUtils()
{
    delete d_ptr;
}

GenFeedbackUtils &GenFeedbackUtils::instance()
{
    static GenFeedbackUtils *instance = new GenFeedbackUtils;
    return *instance;
}

void GenFeedbackUtils::instantFeedback(GenFeedbackUtils::FeedBackType feedback)
{
    GenFeedbackUtils &d = GenFeedbackUtils::instance();
    d.d_ptr->instantFeedback(feedback);
}


