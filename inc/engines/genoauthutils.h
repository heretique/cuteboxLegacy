#ifndef _GENOAUTHUTILS_H_
#define _GENOAUTHUTILS_H_

#include "genoauthcommon.h"

class QString;
class GenOAuthUtils
{
public:

    static QString hmac_sha1(const QString &message, const QString &key);
};

#endif // _GENOAUTHUTILS_H_
