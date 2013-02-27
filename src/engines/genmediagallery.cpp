#ifdef Q_OS_SYMBIAN
#include "genmediagallerysymimpl.h"
#else
#include "genmediagalleryimpl.h"
#endif

#include "genmediagallery.h"

GenMediaGallery::GenMediaGallery() :
    d_ptr(new GenMediaGalleryPrivate(this))
{
}

GenMediaGallery::~GenMediaGallery()
{
    delete d_ptr;
}

void GenMediaGallery::getMediaFiles(QStringList& fileList, MediaFileTypes fileType,
        bool allowMultiple)
{
    Q_D( GenMediaGallery);
    d->getMediaFiles(fileList, fileType, allowMultiple);
}
