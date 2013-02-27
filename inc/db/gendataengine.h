#ifndef GENDATAENGINE_H
#define GENDATAENGINE_H

#include "gendbengine.h"
#include "gentypes.h"

class GenFileInfo;


class GenDataEngine : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent, the parent of the engine.
     */
    GenDataEngine(QObject* parent = 0);

    /*!
     * \brief Destructor
     */
    ~GenDataEngine();

    bool init();
    void purgeAndClose();
    bool removeFilesInfo(QStringList paths);
    bool removeFileInfo(QString path);
    QList<GenFileInfo*> retrieveFilesInfo(QString path);
    QMap<QString, GenFileInfo*> retrieveFilesInfoNameMap(QString path);
    GenFileInfo *retrieveFileInfo(QString path);
    QByteArray retrieveThumbnail(uint thumbhash);
    bool replaceOrAddFileInfo(GenFileInfo* entry);
    bool replaceOrAddThumbnail(uint thumbhash, QByteArray thumbnail);
    void replaceOrAddFilesInfo(QList<GenFileInfo*> filesInfo);
    void syncFileInfo(QString path, QList<GenFileInfo*> filesInfo);
private:
    GenDbEngine _db; /*!< The database engine used to execute queries on the database*/
};

#endif // GENDATAENGINE_H
