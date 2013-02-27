#ifndef GENFILEMANAGER_H
#define GENFILEMANAGER_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QPixmap>

class GenFileManager : public QObject
{
    Q_OBJECT
public:
    GenFileManager(QObject *parent = 0);
    ~GenFileManager();
    void init();
    int spaceLimit() const;
    void setSpaceLimit(int spaceLimit);
    int availableSpace() const;
    void startMakingSpaceAvailable(int neededSpaceAmount);
    bool thumbnailExists(const QString &imagePath);
    QPixmap thumbnail(const QString &imagePath);
    void deleteThumbnail(const QString &imagePath);
    void purgeThumbnails();
    void copy(const QString &source, const QString &destination);
    void move(const QString &source, const QString &destination);
    void remove(const QString &source);
public slots:
    void handleSpaceLimitChanged(int spaceLimit);
    void handleNextInitStep();
private:
    int _spaceLimit; // disk space used for caching thumbnails and
    // downloaded files in megabytes.
    int _neededSpaceAmount; // the space amount needed after the purge
    QString _root;
    QString _thumbnailsFolder;
    QMap<QDateTime, QString> _cache;
};

#endif // GENFILEMANAGER_H
