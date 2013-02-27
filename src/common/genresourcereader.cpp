#include <QPixmap>
#include <QPixmapCache>
#include <QFile>

#include "genresourcereader.h"
#include "genliterals.h"
#include "genlog.h"

GenResourceReader::GenResourceReader() :
_svgRenderer(NULL)
{
}

GenResourceReader::~GenResourceReader()
{
    delete _svgRenderer;
    QPixmapCache::clear();
}

GenResourceReader &GenResourceReader::instance()
{
    static GenResourceReader result;
    return result;
}

void GenResourceReader::setResourceFile(const QString &fileName)
{
    GenResourceReader &d = instance();
    if (d._svgRenderer)
    {
        delete d._svgRenderer;
        d._svgRenderer = NULL;
    }
    d._svgRenderer = new QSvgRenderer(fileName);
}

QPixmap GenResourceReader::pixmapFromSvg(const QString &key, const QSize &size, bool keepAspectRatio)
{
    GenResourceReader &d = instance();
    QPixmap pixmap;
    if (!QPixmapCache::find(key, pixmap)) {
        pixmap = d.generatePixmap(key, size, keepAspectRatio);
    }
    else {
        if (pixmap.size() != size) {
            QPixmapCache::remove(key);
            pixmap = d.generatePixmap(key, size, keepAspectRatio);
        }
    }
    return pixmap;
}

QPixmap GenResourceReader::iconFromResource(const QString &key)
{
    QPixmap pixmap;
    if (!QPixmapCache::find(key, pixmap))
    {
        pixmap = QPixmap(QString(":/icons/%1").arg(key));
        QPixmapCache::insert(key, pixmap);
    }
    return pixmap;
}

QPixmap GenResourceReader::pixmapFromResource(const QString &key)
{
    QPixmap pixmap;
    if (!QPixmapCache::find(key, pixmap))
    {
        pixmap = QPixmap(QString(":/images/%1").arg(key));
        QPixmapCache::insert(key, pixmap);
    }
    return pixmap;
}

QPixmap GenResourceReader::thumbnailForFileExtension(QString extension)
{
    QString thumbnailName = extension + ".png";
    thumbnailName.prepend(":/thumbnails/");
    if (QFile::exists(thumbnailName))
        return QPixmap(thumbnailName);
    else
        return QPixmap(":/thumbnails/bin.png");
}

QPixmap GenResourceReader::generatePixmap(const QString &key, const QSize &size,
    bool keepAspectRatio)
{
    QPixmap pixmap = QPixmap();
    if (_svgRenderer->elementExists(key) && size != QSize(0, 0)) {
        QSize svgImageSize = size;

        if (keepAspectRatio == true) {
            svgImageSize = _svgRenderer->boundsOnElement(key).size().toSize();
            svgImageSize.scale(size, Qt::KeepAspectRatio);
        }

        pixmap = QPixmap(size);
        pixmap.fill(QColor(Qt::transparent));

        QPainter painter(&pixmap);
        QRect drawRect(QPoint((size.width() - svgImageSize.width()) / 2, (size.height()
            - svgImageSize.height()) / 2), svgImageSize);

        _svgRenderer->render(&painter, key, drawRect);
        QPixmapCache::insert(key, pixmap);
    }
    else if (size == QSize(0, 0)) {
        pixmap = QPixmap(size);
        QPixmapCache::insert(key, pixmap);
    }
    else {
        QLOG_DEBUG("Svg element:" << key << "not found");
    }
    return pixmap;
}

