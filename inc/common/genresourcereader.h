#ifndef RESOURCEREADER_H
#define RESOURCEREADER_H

#include <QPixmap>
#include <QMap>
#include <QSize>
#include <QSvgRenderer>
#include <QPainter>

class QPixmap;
class QSvgRenderer;

class GenResourceReader : public QObject
{
    Q_OBJECT
public:

    static GenResourceReader &instance();
    
    static void setResourceFile(const QString &fileName);

    /*!
     * \brief Getter for a pixmap from the pixmap cache corresponding to the key from config file     *
     * \detail If the pixmap is not in the pixmap cache it will insert it
     * \param key, a const QString&. the key from config file
     * \param size, a const QSize& argument. The desired image size.
     * \param keepAspectRatio, a flag to whether or not to keep aspect ratio.
     * \return Pixmap
     */
    static QPixmap pixmapFromSvg(const QString &key, const QSize &size,
        bool keepAspectRatio = true);

    static QPixmap iconFromResource(const QString &key);

    static QPixmap pixmapFromResource(const QString &key);

    static QPixmap thumbnailForFileExtension(QString extension);

private:
    /*!
     * \brief Reads image data from resource file and renders it into pixmap
     * \param key, a const QString&. The image id.
     * \param size, a const QSize& argument. The desired image size.
     * \param keepAspectRatio, a flag to whether or not to keep aspect ratio.
     * \return Pixmap&
     */
    QPixmap generatePixmap(const QString &key, const QSize &size,
        bool keepAspectRatio = true);

    /*!
     * \brief Constructor
     */
    GenResourceReader();

    /*!
     * \brief Destructor
     */
    ~GenResourceReader();
    
private: // DATA
    QSvgRenderer *_svgRenderer; /*! svg renderer used for drawing the pixmaps out of svg resources
*/
};

#endif
