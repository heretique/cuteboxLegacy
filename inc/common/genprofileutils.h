#ifndef PROFILEUTILS_H
#define PROFILEUTILS_H

#include <QObject>

class GenProfileUtilsPrivate;
class GenProfileUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Profile profile READ profile)
    Q_ENUMS(Profile)
public:

    GenProfileUtils(QObject *parent = 0);
    virtual ~GenProfileUtils();
    enum Profile { Unknown, Silent, Meeting, General, Offline, Outdoor };
    Profile profile() const;
    void monitor();
    void stopmonitor();

Q_SIGNALS:
    void profileChanged(GenProfileUtils::Profile profile);

protected:
    GenProfileUtilsPrivate * const d_ptr;
private:
    void profileChangedCallback(int profile);
    Q_DECLARE_PRIVATE(GenProfileUtils)
};

#endif // PROFILEUTILS_H
