#include "genfeedbackutilsimpl.h"

#ifdef Q_OS_WIN32
GenFeedbackUtilsPrivate::GenFeedbackUtilsPrivate(GenFeedbackUtils *parent)
    : q_ptr(parent)
{

}

GenFeedbackUtilsPrivate::~GenFeedbackUtilsPrivate()
{

}



void GenFeedbackUtilsPrivate::instantFeedback(GenFeedbackUtils::FeedBackType feedback)
{

}
#endif
