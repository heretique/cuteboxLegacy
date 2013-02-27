
#ifndef MEDIAGALLERYIMPL_H_
#define MEDIAGALLERYIMPL_H_

#include "genmediagallery.h"
// CLASS DECLARATION

class GenMediaGalleryPrivate
{
    Q_DECLARE_PUBLIC(GenMediaGallery)
public:
    // Constructors and destructor
    GenMediaGalleryPrivate(GenMediaGallery *parent) : q_ptr(parent){};
    ~GenMediaGalleryPrivate(){};

public:
    /*!
     * \brief Retrieves the selected files list from the gallery
     * \param fileList The file list
     * \param fileTypes The type of the files
     * \param allowMultiple allow or disallow multiple selection
     */
    void getMediaFiles(QStringList& fileList,
            GenMediaGallery::MediaFileTypes fileTypes, bool allowMultiple);
public:
    GenMediaGallery * const q_ptr;
};

#endif /* MEDIAGALLERYIMPL_H_ */
