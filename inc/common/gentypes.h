#ifndef GENTYPES_H_
#define GENTYPES_H_


#include <QString>
#include <QHash>
#include <QDateTime>

#include "genenums.h"


typedef int FileId;
typedef int ActionId;

typedef struct GenWSRequestInfo
{
    int _id;
    QString _domain;
    QString _path;
    GenHTTPRequestType _httpType;
    GenWSAuthenticationType _authType;
    bool _isSecure;
    bool _needsParams;
} _GenWSRequestInfo;

typedef struct GenAccountInfo
{
   QString _uid;
   QString _country;
   QString _displayName;
   qint64 _sharedSpace;
   qint64 _spaceLimit;
   qint64 _usedSpace;
} _GenAccountInfo;

typedef QHash<QString, QDateTime> FilesList;

#endif /* GENTYPES_H_ */
