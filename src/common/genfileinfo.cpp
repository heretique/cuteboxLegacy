#include "genfileinfo.h"
#include "genconstants.h"

GenFileInfo::GenFileInfo() :
        _path(QString()),
        _size(0),
        _isDir(false),
        _hasThumb(false),
        _isFavorite(false),
        _modifiedTime(QDateTime()),
        _iconName(QString()),
        _hash(QString()),
        _thumbhash(0),
        _icon(QPixmap()),
        _iconKey(QPixmapCache::Key())
{

}

GenFileInfo::~GenFileInfo()
{

}

bool fileInfoLessThan(const GenFileInfo *left, const GenFileInfo *right)
{
    if (left->_isDir && !right->_isDir)
        return true;
    else if (!left->_isDir && right->_isDir)
        return false;

    return left->_path.toLower() < right->_path.toLower();
}
