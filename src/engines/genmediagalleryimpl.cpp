#include <QFileDialog>
#include "genmediagalleryimpl.h"
#include "genlog.h"

void GenMediaGalleryPrivate::getMediaFiles(QStringList& fileList,
        GenMediaGallery::MediaFileTypes fileTypes, bool allowMultiple)
{
    fileList = QFileDialog::getOpenFileNames(
          NULL,
          "Select Images",
          QDir::currentPath(),
          "Images (*.png *.gif *.jpg);;All files (*.*)");
      if( !fileList.isEmpty() )
      {
        QLOG_DEBUG( fileList.join(",").toAscii() );
      }
}
