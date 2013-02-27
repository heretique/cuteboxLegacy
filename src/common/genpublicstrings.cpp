#include <QObject>

#include "genpublicstrings.h"

//Main screen
QString GenPublicStrings::STR_TEST = "";
QString GenPublicStrings::STR_WELCOME = "";
QString GenPublicStrings::STR_CHOOSE_DOWNLOAD_FILES = "";
QString GenPublicStrings::STR_CONFIRM_DOWNLOAD_FILES = "";
QString GenPublicStrings::STR_CHOOSE_FILES = "";
QString GenPublicStrings::STR_CONFIRM_COPY_FILES = "";
QString GenPublicStrings::STR_CONFIRM_MOVE_FILES = "";
QString GenPublicStrings::STR_CONFIRM_DELETE_FILES = "";
QString GenPublicStrings::STR_CONFIRM_UPLOAD_FILES = "";
QString GenPublicStrings::STR_UPLOAD_SELECTED_COUNT = "";
QString GenPublicStrings::STR_CHOOSE_UPLOAD_FILES = "";
QString GenPublicStrings::STR_CREATING_FOLDER = "";
QString GenPublicStrings::STR_CHOOSE_NEW_DOWNLOAD_LOCATION = "";
QString GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS = "";


GenPublicStrings::GenPublicStrings()
    {
    retranslateStrings();
    }

void GenPublicStrings::retranslateStrings()
    {
    //Main screen
    STR_TEST = QObject::tr("TEST", "");
    STR_WELCOME = QObject::tr("Welcome to cuteBox");
    STR_CHOOSE_DOWNLOAD_FILES = QObject::tr("Choose the files you want to download.");
    STR_CONFIRM_DOWNLOAD_FILES = QObject::tr("Download %1 file(s)?");
    STR_CHOOSE_FILES = QObject::tr("Choose the files or folders and then chose your action.");
    STR_CONFIRM_COPY_FILES = QObject::tr("Copy %1 file(s) to: %2 ?");
    STR_CONFIRM_MOVE_FILES = QObject::tr("Move %1 file(s) to: %2 ?");
    STR_CONFIRM_DELETE_FILES = QObject::tr("Delete %1 file(s) ?");
    STR_CHOOSE_UPLOAD_FILES = QObject::tr("Choose the files you want to upload.");
    STR_UPLOAD_SELECTED_COUNT = QObject::tr("%1 file(s) selected for upload.");
    STR_CONFIRM_UPLOAD_FILES = QObject::tr("Upload %1 file(s) to: %2 ?");
    STR_CREATING_FOLDER = QObject::tr("Creating folder...");
    STR_CHOOSE_NEW_DOWNLOAD_LOCATION = QObject::tr("Choose new download location");
    STR_ANOTHER_ACTION_IN_PROGRESS = QObject::tr("<b>!</b>Another action already in progress");
    }
