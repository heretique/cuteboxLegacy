#include "genprofileutilsimpl.h"

#ifdef Q_OS_WIN32
GenProfileUtilsPrivate::GenProfileUtilsPrivate(GenProfileUtils *parent)
    : q_ptr(parent)
{

}

GenProfileUtilsPrivate::~GenProfileUtilsPrivate()
{

}

GenProfileUtils::Profile GenProfileUtilsPrivate::profile() const
{
    return GenProfileUtils::Unknown;
}

void GenProfileUtilsPrivate::monitor()
{

}

void GenProfileUtilsPrivate::stopmonitor()
{

}

#endif
