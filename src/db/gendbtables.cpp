#include "gendbtables.h"

QString GenDbStrings::DBNAME = "cutebox.db";
QString GenDbStrings::TABLENAME = "filesinfo";
QString GenDbStrings::CREATEFILESINFOTABLE = "create table filesinfo (path text unique collate nocase primary key, size integer, isdir boolean, hasthumb boolean, isfavorite boolean, modified datetime, icon varchar(32), hash varchar(32), thumbhash integer)";
QString GenDbStrings::CREATETHUMBNAILSTABLE = "create table thumbnails (thumbhash integer unique primary key, thumbnail blob)";
QString GenDbStrings::DROPFILESINFOTABLE = "drop table filesinfo";
QString GenDbStrings::DROPTHUMBNAILSTABLE = "drop table thumbnails";
