#ifndef DBSTRINGS_H_
#define DBSTRINGS_H_

// System includes
#include <QString>

// User includes


// Forward declarations


// Class declaration
class GenDbStrings
    {
public:
    // database name
    static QString DBNAME;
    static QString TABLENAME;
    //creation sql statements
    static QString CREATEFILESINFOTABLE;
    static QString CREATETHUMBNAILSTABLE;
    static QString DROPFILESINFOTABLE;
    static QString DROPTHUMBNAILSTABLE;
    };

#endif /* DBSTRINGS_H_ */
