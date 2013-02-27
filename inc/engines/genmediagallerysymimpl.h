#ifndef MEDIAGALLERYSYMIMPL_H
#define MEDIAGALLERYSYMIMPL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "genmediagallery.h"
// CLASS DECLARATION

class GenMediaGalleryPrivate : public CBase
{
    Q_DECLARE_PUBLIC(GenMediaGallery)
public:
    // Constructors and destructor
    GenMediaGalleryPrivate(GenMediaGallery *parent);
    ~GenMediaGalleryPrivate();

public:
    /*!
     * \brief Retrieves the selected files list from the gallery
     * \param fileList The file list
     * \param fileTypes The type of the files
     * \param allowMultiple allow or disallow multiple selection
     */
    void getMediaFiles(QStringList& fileList,
            GenMediaGallery::MediaFileTypes fileType, bool allowMultiple);
public:
    GenMediaGallery * const q_ptr;
};

#endif // MEDIAGALLERYSYMIMPL_H
