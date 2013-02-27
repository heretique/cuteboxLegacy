// System includes
#include <QtSql>

// User includes
#include "gendbengine.h"
#include "gendbtables.h"
#include "genlog.h"

GenDbEngine::GenDbEngine()
{

}

GenDbEngine::~GenDbEngine()
{
    closedb();
}

bool GenDbEngine::opendb()
{
    return createConnection();
}

void GenDbEngine::closedb()
{
    return _db.close();
}

void GenDbEngine::purgedb()
{
    dropTables();
}

bool GenDbEngine::createConnection()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(GenDbStrings::DBNAME);

    if (!_db.open())
    {
        QLOG_ERROR("Failed opening database: " << _db.lastError());
        return false;
    }
    QStringList tables = _db.tables();
    if (!tables.isEmpty())
    {
        if (-1 != tables.indexOf(GenDbStrings::TABLENAME))
        {
            return true;
        }
    }
    // creates tables inside db
    createTables();
    createTriggers();

#ifdef _TEST_
    createTestData();
#endif
    return true;
}

bool GenDbEngine::createTables()
{
    QSqlQuery query;
    QSqlError error;
    if (!query.exec(GenDbStrings::CREATEFILESINFOTABLE))
    {
        error = query.lastError();
        QLOG_ERROR("Failed creating filesInfo table: " << error);
    }
    if (!query.exec(GenDbStrings::CREATETHUMBNAILSTABLE))
    {
        error = query.lastError();
        QLOG_ERROR("Failed creating thumbnail table: " << error);
    }

    return true;
}

bool GenDbEngine::createTriggers()
{
    QSqlQuery query;
    QSqlError error;
    bool success = execSql("CREATE TRIGGER delete_thumbnail DELETE ON filesinfo "
                           "BEGIN "
                           " DELETE FROM thumbnails WHERE thumbnails.thumbhash = thumbhash; "
                           "END;");
    if (!success)
    {
        error = query.lastError();
        QLOG_ERROR("Failed creating filesInfo table: " << error);
    }
    return success;
}

bool GenDbEngine::dropTables()
{
    QSqlQuery query;
    QSqlError error;
    if (!query.exec(GenDbStrings::DROPFILESINFOTABLE))
    {
        error = query.lastError();
        QLOG_ERROR("Failed dropping filesInfo table: " << error);
    }
    if (!query.exec(GenDbStrings::DROPTHUMBNAILSTABLE))
    {
        error = query.lastError();
        QLOG_ERROR("Failed dropping thumbnail table: " << error);
    }

    return true;
}

#ifdef _TEST_
void GenDbEngine::createTestData()
{
    //    QSqlQuery query;

    //    // insert entry data
    //    query.exec(
    //            QString("insert into entries values(0, '<b><p><pb>Entry text 1</pb><ps>0</ps><pt>06:11</pt>")
    //            + QString("<pr>0</pr></p><p><pb>Entry text 2</pb><ps>1</ps><pt>07:01</pt><pr>0</pr></p><p><pb>")
    //            + QString("Entry text 3</pb><ps>1</ps><pt>08:32</pt><pr>0</pr></p><p><pb>Entry text 4</pb><ps>")
    //            + QString("0</ps><pt>08:45</pt><pr>0</pr></p></b>', date('now'), 1, 0)"));
}
#endif

bool GenDbEngine::beginTransaction()
{
    if (!_db.transaction())
    {
        return false;
    }
    return true;
}

bool GenDbEngine::commitTransaction()
{
    if (!_db.commit())
    {
        return false;
    }
    return true;
}

void GenDbEngine::rollbackTransaction()
{
    _db.rollback();
}

bool GenDbEngine::execSql(const QString& sql, QVariantList* values,
                          QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

bool GenDbEngine::execSql(const QString& sql, const QVariant& boundValue1,
                          QVariantList* values, QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql, boundValue1);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

bool GenDbEngine::execSql(const QString& sql, const QVariant& boundValue1,
                          const QVariant& boundValue2, QVariantList* values,
                          QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql, boundValue1, boundValue2);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

bool GenDbEngine::execSql(const QString& sql, const QVariant& boundValue1,
                          const QVariant& boundValue2, const QVariant& boundValue3,
                          QVariantList* values, QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql, boundValue1, boundValue2, boundValue3);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

bool GenDbEngine::execSql(const QString& sql, const QVariant& boundValue1,
                          const QVariant& boundValue2, const QVariant& boundValue3,
                          const QVariant& boundValue4, QVariantList* values,
                          QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql, boundValue1, boundValue2, boundValue3,
                                boundValue4);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

bool GenDbEngine::execSql(const QString& sql,
                          const QVariantList& boundValues, QVariantList* values,
                          QVariant *lastInsertId)
{
    QSqlQuery query = execQuery(sql, boundValues);
    if (!query.isActive())
        return false;
    if (lastInsertId)
        (*lastInsertId) = query.lastInsertId();
    if (values)
        (*values) = readToList(query);
    return true;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql,
                                 const QVariant& boundValue1)
{
    QSqlQuery query = prepareQuery(sql);
    query.bindValue(0, boundValue1);
    exec(query);
    return query;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql,
                                 const QVariant& boundValue1, const QVariant& boundValue2)
{
    QSqlQuery query = prepareQuery(sql);
    query.bindValue(0, boundValue1);
    query.bindValue(1, boundValue2);
    exec(query);
    return query;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql,
                                 const QVariant& boundValue1, const QVariant& boundValue2,
                                 const QVariant& boundValue3)
{
    QSqlQuery query = prepareQuery(sql);
    query.bindValue(0, boundValue1);
    query.bindValue(1, boundValue2);
    query.bindValue(2, boundValue3);
    exec(query);
    return query;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql,
                                 const QVariant& boundValue1, const QVariant& boundValue2,
                                 const QVariant& boundValue3, const QVariant& boundValue4)
{
    QSqlQuery query = prepareQuery(sql);
    query.bindValue(0, boundValue1);
    query.bindValue(1, boundValue2);
    query.bindValue(2, boundValue3);
    query.bindValue(3, boundValue4);
    exec(query);
    return query;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql,
                                 const QVariantList& boundValues)
{
    QSqlQuery query = prepareQuery(sql);
    for (int i = 0; i < boundValues.size(); ++i)
    {
        query.bindValue(i, boundValues[i]);
    }
    exec(query);
    return query;
}

QSqlQuery GenDbEngine::execQuery(const QString& sql)
{
    QSqlQuery query = prepareQuery(sql);
    exec(query);
    return query;
}

bool GenDbEngine::exec(QSqlQuery& query)
{
    if (!query.exec())
    {
        QLOG_ERROR( "Failure executing query: ");
        QLOG_ERROR( query.executedQuery());
        QLOG_ERROR(query.lastError().text() << query.lastError().number());
        QLOG_ERROR("Bound values: " << query.boundValues().values());
        return false;
    }
    return true;
}

bool GenDbEngine::execBatch(QSqlQuery& query)
{
    if (!query.execBatch())
    {
        // Use DatabaseAccess::lastError?
        QLOG_ERROR("Failure executing batch query: ");
        QLOG_ERROR(query.executedQuery());
        QLOG_ERROR(query.lastError().text() << query.lastError().number());
        QLOG_ERROR("Bound values: " << query.boundValues().values());
        return false;
    }
    return true;
}

QSqlQuery GenDbEngine::prepareQuery(const QString& sql)
{
    QSqlQuery query(_db);
    query.setForwardOnly(true);
    query.prepare(sql);
    return query;
}

QVariantList GenDbEngine::readToList(QSqlQuery& query)
{
    QVariantList list;

    QSqlRecord record = query.record();
    int count = record.count();

    while (query.next())
    {
        for (int i = 0; i < count; ++i)
            list << query.value(i);
    }
    return list;
}

QString GenDbEngine::lastError()
{
    return _db.lastError().text();
}
