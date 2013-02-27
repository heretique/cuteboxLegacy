#ifndef MEDIAGALLERY_H_
#define MEDIAGALLERY_H_

#include <QObject>

class GenMediaGalleryPrivate;
class GalleryObserver;

class GenMediaGallery: public QObject
{
    Q_OBJECT
    
public:
    GenMediaGallery();
    ~GenMediaGallery();

    enum MediaFileTypes
    {
        ImageFile = 0x00000001,
        VideoFile = 0x00000002,
        AudioFile = 0x00000004
    };

    /*!
     * \brief Retrieves the selected files list from the gallery
     * \param fileList The file list
     * \param fileTypes The type of the files
     * \param allowMultiple allow or disallow multiple selection
     */
    void getMediaFiles(QStringList& fileList, MediaFileTypes fileType,
            bool allowMultiple);
    
    
protected:
    GenMediaGalleryPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GenMediaGallery)
};

#endif /* MEDIAGALLERY_H_ */
