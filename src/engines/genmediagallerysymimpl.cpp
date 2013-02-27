#include <MGFetch.h>
#include <badesca.h>
#include <QStringList>
#include "genmediagallerysymimpl.h"
#include "genlog.h"


GenMediaGalleryPrivate::GenMediaGalleryPrivate(GenMediaGallery *parent) :
    q_ptr(parent)
{
}

GenMediaGalleryPrivate::~GenMediaGalleryPrivate()
{
}

void GenMediaGalleryPrivate::getMediaFiles(QStringList& fileList,
                                           GenMediaGallery::MediaFileTypes fileType, bool allowMultiple)
{
    // Create array of descriptors for the selected files
    CDesCArrayFlat* fileArray = new (ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL(fileArray);
    TBuf<32> lHeading;
    TMediaFileType type;

    switch (fileType)
    {
    case (GenMediaGallery::ImageFile):
    {
        lHeading.Append(_L("Select Images"));
        type = EImageFile;
        break;
    }
    case (GenMediaGallery::VideoFile):
    {
        lHeading.Append(_L("Select Videos"));
        type = EVideoFile;
        break;
    }
    case (GenMediaGallery::AudioFile):
    {
        lHeading.Append(_L("Select Audio"));
        type = EAudioFile;
        break;
    }
    default:
    {
        type = EImageFile;
        break;
    }
    }
    QLOG_DEBUG("Begin MGFetch");
    // Open the dialog. this is overloaded
    TBool ret = KErrGeneral;
    TRAP_IGNORE(ret = MGFetch::RunL(*fileArray, // When dialog is closed, fileArray contains selected files
                              type, // Displays only media files of type aMediaType
                              allowMultiple,
                              KNullDesC,
                              lHeading));
    QLOG_DEBUG("End MGFetch");
    if (ret && fileArray)
    {
        for (int i = 0; i < fileArray->Count(); i++)
        {
            QString s1 = QString((QChar*) fileArray->MdcaPoint(i).Ptr(),
                                 fileArray->MdcaPoint(i).Length());
            fileList << s1;
        }
        if( !fileList.isEmpty() )
        {
            QLOG_DEBUG( fileList.join(",").toAscii() );
        }
    }

    CleanupStack::PopAndDestroy(); // fileArray
}
