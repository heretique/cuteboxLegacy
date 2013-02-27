#include "genprofileutils.h"
#ifdef Q_OS_SYMBIAN
#include "genprofileutilssymimpl.h"
#else
#include "genprofileutilsimpl.h"
#endif


GenProfileUtils::GenProfileUtils(QObject *parent) : QObject(parent),
d_ptr(new GenProfileUtilsPrivate(this))
{

}

GenProfileUtils::~GenProfileUtils()
{
    delete d_ptr;
}


GenProfileUtils::Profile GenProfileUtils::profile() const
{
    Q_D(const GenProfileUtils);
    return d->profile();
}

void GenProfileUtils::monitor()
{
    Q_D(GenProfileUtils);
    return d->monitor();
}

void GenProfileUtils::stopmonitor()
{
    Q_D(GenProfileUtils);
    return d->stopmonitor();
}

void GenProfileUtils::profileChangedCallback(int profile)
{
    Profile activeProfile = Unknown;
      switch ( profile )
      {
      case 0: // General profile
          {
              activeProfile = General;
              break;
          }
      case 1: // Silent profile
          {
              activeProfile = Silent;
              break;
          }
      case 2: // Meeting profile
          {
              activeProfile = Meeting;
              break;
          }
      case 3: // Outdoor profile
          {
              activeProfile = Outdoor;
              break;
          }
      default:
          {
              break;
          }
      }
    emit profileChanged(activeProfile);
}
