#ifndef GENFILEINFO_H
#define GENFILEINFO_H

#include <QDateTime>
#include <QPixmap>
#include <QPixmapCache>
#include "gentypes.h"

class GenFileInfo
{
public:
    GenFileInfo();
    ~GenFileInfo();
private:
    Q_DISABLE_COPY(GenFileInfo)
public:
    QString _path;
    int _size;
    bool _isDir;
    bool _hasThumb;
    bool _isFavorite;
    QDateTime _modifiedTime;
    QString _iconName;
    QString _hash;
    uint _thumbhash;
    QPixmap _icon;
    QPixmapCache::Key _iconKey;
};

bool fileInfoLessThan(const GenFileInfo *left, const GenFileInfo *right);

#endif // GENFILEINFO_H
