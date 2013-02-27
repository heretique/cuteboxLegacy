#include "gendataengine.h"
#include "genfileinfo.h"
#include "genconstants.h"
#include "genlog.h"

#include <QApplication>
#include <QSqlQuery>
#include <QSqlRecord>


GenDataEngine::GenDataEngine(QObject* parent) :
    QObject(parent)
{

}

GenDataEngine::~GenDataEngine()
{
    _db.closedb();
}

bool GenDataEngine::init()
{
    return _db.opendb();
}

void GenDataEngine::purgeAndClose()
{
    _db.purgedb();
    _db.closedb();
}

bool GenDataEngine::removeFilesInfo(QStringList paths)
{
    QLOG_DEBUG("GenDataEngine::removeFilesInfo");
    if (_db.beginTransaction())
    {
        for (int i = 0;  i < paths.size();  ++i)
        {
            removeFileInfo(paths.at(i));
        }
    }
    return _db.commitTransaction();
}

bool GenDataEngine::removeFileInfo(QString path)
{
    QLOG_DEBUG("GenDataEngine::removeFileInfo - path = " << path);
    QString sqlQueryString = QString(
                "delete from filesinfo where path like \'%1%\'").arg(path);
    return _db.execSql(sqlQueryString);
}

QList<GenFileInfo*> GenDataEngine::retrieveFilesInfo(QString path)
{
    QList<GenFileInfo*> result;

    QString sqlQueryString = QString(
                "select * from filesinfo where path like \'%1/%\' and path not like \'%2/%/%\'").arg(path, path);
    QSqlQuery fileInfoQuery = _db.execQuery(sqlQueryString);
    if (fileInfoQuery.isActive())
    {
        QSqlRecord fileInfoRec = fileInfoQuery.record();
        while(fileInfoQuery.next())
        {
            GenFileInfo *fileInfo = new GenFileInfo;
            fileInfo->_path = fileInfoQuery.value(fileInfoRec.indexOf("path")).toString();
            fileInfo->_size = fileInfoQuery.value(fileInfoRec.indexOf("size")).toInt();
            fileInfo->_isDir = fileInfoQuery.value(fileInfoRec.indexOf("isdir")).toBool();
            fileInfo->_hasThumb = fileInfoQuery.value(fileInfoRec.indexOf("hasthumb")).toBool();
            fileInfo->_isFavorite = fileInfoQuery.value(fileInfoRec.indexOf("isfavorite")).toBool();
            fileInfo->_modifiedTime = fileInfoQuery.value(fileInfoRec.indexOf("modified")).toDateTime();
            fileInfo->_modifiedTime.setTimeSpec(Qt::UTC);
            fileInfo->_iconName = fileInfoQuery.value(fileInfoRec.indexOf("icon")).toString();
            fileInfo->_hash = fileInfoQuery.value(fileInfoRec.indexOf("hash")).toString();
            fileInfo->_thumbhash = fileInfoQuery.value(fileInfoRec.indexOf("thumbhash")).toUInt();
            result.append(fileInfo);
        }
    }
    return result;
}

QMap<QString, GenFileInfo*> GenDataEngine::retrieveFilesInfoNameMap(QString path)
{
    QMap<QString, GenFileInfo*> result;

    QString sqlQueryString = QString(
                "select * from filesinfo where path like \'%1/%\' and path not like \'%2/%/%\'").arg(path, path);
    QSqlQuery fileInfoQuery = _db.execQuery(sqlQueryString);
    if (fileInfoQuery.isActive())
    {
        QSqlRecord fileInfoRec = fileInfoQuery.record();
        while(fileInfoQuery.next())
        {
            GenFileInfo *fileInfo = new GenFileInfo;
            fileInfo->_path = fileInfoQuery.value(fileInfoRec.indexOf("path")).toString();
            fileInfo->_size = fileInfoQuery.value(fileInfoRec.indexOf("size")).toInt();
            fileInfo->_isDir = fileInfoQuery.value(fileInfoRec.indexOf("isdir")).toBool();
            fileInfo->_hasThumb = fileInfoQuery.value(fileInfoRec.indexOf("hasthumb")).toBool();
            fileInfo->_isFavorite = fileInfoQuery.value(fileInfoRec.indexOf("isfavorite")).toBool();
            fileInfo->_modifiedTime = fileInfoQuery.value(fileInfoRec.indexOf("modified")).toDateTime();
            fileInfo->_modifiedTime.setTimeSpec(Qt::UTC);
            fileInfo->_iconName = fileInfoQuery.value(fileInfoRec.indexOf("icon")).toString();
            fileInfo->_hash = fileInfoQuery.value(fileInfoRec.indexOf("hash")).toString();
            fileInfo->_thumbhash = fileInfoQuery.value(fileInfoRec.indexOf("thumbhash")).toUInt();
            result.insert(fileInfo->_path, fileInfo);
        }
    }
    return result;
}


GenFileInfo *GenDataEngine::retrieveFileInfo(QString path)
{
    GenFileInfo *fileInfo = NULL;

    QString sqlQueryString = QString(
                "select * from filesinfo where path = \"%1\"").arg(path);
    QSqlQuery fileInfoQuery = _db.execQuery(sqlQueryString);
    if (fileInfoQuery.isActive())
    {
        QSqlRecord fileInfoRec = fileInfoQuery.record();
        if(fileInfoQuery.next())
        {
            fileInfo = new GenFileInfo;
            fileInfo->_path = fileInfoQuery.value(fileInfoRec.indexOf("path")).toString();
            fileInfo->_size = fileInfoQuery.value(fileInfoRec.indexOf("size")).toInt();
            fileInfo->_isDir = fileInfoQuery.value(fileInfoRec.indexOf("isdir")).toBool();
            fileInfo->_hasThumb = fileInfoQuery.value(fileInfoRec.indexOf("hasthumb")).toBool();
            fileInfo->_isFavorite = fileInfoQuery.value(fileInfoRec.indexOf("isfavorite")).toBool();
            fileInfo->_modifiedTime = fileInfoQuery.value(fileInfoRec.indexOf("modified")).toDateTime();
            fileInfo->_modifiedTime.setTimeSpec(Qt::UTC);
            fileInfo->_iconName = fileInfoQuery.value(fileInfoRec.indexOf("icon")).toString();
            fileInfo->_hash = fileInfoQuery.value(fileInfoRec.indexOf("hash")).toString();
            fileInfo->_thumbhash = fileInfoQuery.value(fileInfoRec.indexOf("thumbhash")).toUInt();
        }
    }
    return fileInfo;
}

QByteArray GenDataEngine::retrieveThumbnail(uint thumbhash)
{
    QByteArray thumbnail;

    QString sqlQueryString = QString(
                "select thumbnail from thumbnails where thumbhash = \"%1\"").arg(thumbhash);
    QSqlQuery thumbnailQuery = _db.execQuery(sqlQueryString);
    if (thumbnailQuery.isActive())
    {
        QSqlRecord thumbnailRec = thumbnailQuery.record();
        if(thumbnailQuery.next())
        {
            thumbnail = thumbnailQuery.value(thumbnailRec.indexOf("thumbnail")).toByteArray();
        }
    }
    return thumbnail;
}

bool GenDataEngine::replaceOrAddFileInfo(GenFileInfo* fileInfo)
{
    bool success = false;

    success = _db.execSql(
                "replace into filesinfo (path, size, isdir, hasthumb, isfavorite, modified, icon, hash, thumbhash) values (?, ?, ?, ?, ?, ?, ?, ?, ?)",
                QVariantList() << fileInfo->_path
                << fileInfo->_size
                << fileInfo->_isDir
                << fileInfo->_hasThumb
                << fileInfo->_isFavorite
                << fileInfo->_modifiedTime
                << fileInfo->_iconName
                << fileInfo->_hash
                << fileInfo->_thumbhash);
    return success;
}

bool GenDataEngine::replaceOrAddThumbnail(uint thumbhash, QByteArray thumbnail)
{
    bool success = false;
    success = _db.execSql(
                "replace into thumbnails (thumbhash, thumbnail) values (?, ?)",
                QVariantList() << thumbhash
                << thumbnail);
    return success;
}

void GenDataEngine::replaceOrAddFilesInfo(QList<GenFileInfo *> filesInfo)
{
    if (_db.beginTransaction())
    {
        for (int i = 0; i < filesInfo.size(); ++i)
        {
            replaceOrAddFileInfo(filesInfo.at(i));
        }
    }
    _db.commitTransaction();
}

void GenDataEngine::syncFileInfo(QString path, QList<GenFileInfo *> filesInfo)
{
    // first get the list of files info stored in db
    QMap<QString, GenFileInfo*> dbCache = retrieveFilesInfoNameMap(path);
    GenFileInfo *fi = NULL;
    QList<GenFileInfo*> newList;
    QList<GenFileInfo*> updateList;
    QStringList orphaned;
    QMap<QString, GenFileInfo*>::iterator it;
    for (int i = 0; i < filesInfo.size(); ++i)
    {
        fi = filesInfo.at(i);
        it = dbCache.find(fi->_path);
        if (it != dbCache.end())
        {
            // found this file in cache
            if (fi->_modifiedTime != it.value()->_modifiedTime)
            {
                updateList.append(fi);
            }
            // we delete the file info we found
            // what remains in dbCache map are the orphaned files info that we
            // need to delete from cache
            delete it.value();
            dbCache.erase(it);
        }
        else
        {
            updateList.append(fi);
        }
    }
    // append orphaned ids and delete dbCache resources
    for (it = dbCache.begin(); it != dbCache.end(); ++it)
    {
        orphaned.append(it.value()->_path);
        delete it.value();
    }
    dbCache.clear();
    // remove orphaned files info from db
    removeFilesInfo(orphaned);
    // update modified files info and add new ones
    replaceOrAddFilesInfo(updateList);
}
