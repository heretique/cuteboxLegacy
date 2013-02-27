#ifndef GENDBENGINE_H
#define GENDBENGINE_H

// System includes
#include <QtSql>
// User includes

#define _TEST_
// Forward declarations

// Class declaration
class QVariant;

class GenDbEngine
    {
public:
    /**
     *
     */
    GenDbEngine();
    /**
     *
     */
    ~GenDbEngine();

    bool opendb();

    void closedb();

    void purgedb();

    bool beginTransaction();

    bool commitTransaction();

    void rollbackTransaction();

    /**
     * Executes the SQL statement, and write the returned data into the values list.
     * If you are not interested in the returned data, set values to 0.
     * Methods are provided for up to four bound values (positional binding), or for a list of bound values.
     * If you want the last inserted id (and your query is suitable), set lastInsertId to the address of a QVariant.
     */
    bool execSql(const QString& sql, QVariantList* values = 0,
            QVariant *lastInsertId = 0);
    bool execSql(const QString& sql, const QVariant& boundValue1,
            QVariantList* values = 0, QVariant *lastInsertId = 0);
    bool execSql(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2, QVariantList* values = 0,
            QVariant *lastInsertId = 0);
    bool execSql(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2, const QVariant& boundValue3,
            QVariantList* values = 0, QVariant *lastInsertId = 0);
    bool execSql(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2, const QVariant& boundValue3,
            const QVariant& boundValue4, QVariantList* values = 0,
            QVariant *lastInsertId = 0);
    bool execSql(const QString& sql, const QVariantList& boundValues,
            QVariantList* values = 0, QVariant *lastInsertId = 0);

    /**
     * Executes the statement and returns the query object.
     * Methods are provided for up to four bound values (positional binding), or for a list of bound values.
     */
    QSqlQuery execQuery(const QString& sql);
    QSqlQuery execQuery(const QString& sql, const QVariant& boundValue1);
    QSqlQuery execQuery(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2);
    QSqlQuery execQuery(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2, const QVariant& boundValue3);
    QSqlQuery execQuery(const QString& sql, const QVariant& boundValue1,
            const QVariant& boundValue2, const QVariant& boundValue3,
            const QVariant& boundValue4);
    QSqlQuery execQuery(const QString& sql, const QVariantList& boundValues);

    /**
     * Calls exec/execBatch on the query, and handles debug output if something went wrong
     */
    bool exec(QSqlQuery& query);
    bool execBatch(QSqlQuery& query);

    QSqlQuery prepareQuery(const QString& sql);
    QVariantList readToList(QSqlQuery& query);

    QString lastError();

private:
    /**
     *
     */
    bool createConnection();

    bool createTables();

    bool createTriggers();

    bool dropTables();

#ifdef _TEST_
    void createTestData();
#endif

public:
    QSqlDatabase _db;
    };

#endif /* GENDBENGINE_H */
