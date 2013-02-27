#ifndef PUBLICSTRINGS_H_
#define PUBLICSTRINGS_H_

#include <QString>

class GenPublicStrings
{
public:
    /*!
     * \brief Fill string variables with localized content
     */
    static void retranslateStrings();

public:
    //Main screen
    static QString STR_TEST; /*!< No images String. */
    static QString STR_WELCOME;
    static QString STR_CHOOSE_DOWNLOAD_FILES;
    static QString STR_CONFIRM_DOWNLOAD_FILES;
    static QString STR_CHOOSE_FILES;
    static QString STR_CONFIRM_COPY_FILES;
    static QString STR_CONFIRM_MOVE_FILES;
    static QString STR_CONFIRM_DELETE_FILES;
    static QString STR_CHOOSE_UPLOAD_FILES;
    static QString STR_UPLOAD_SELECTED_COUNT;
    static QString STR_CONFIRM_UPLOAD_FILES;
    static QString STR_CREATING_FOLDER;
    static QString STR_CHOOSE_NEW_DOWNLOAD_LOCATION;
    static QString STR_ANOTHER_ACTION_IN_PROGRESS;
private:
    /*!
     * \brief Constructor
     */
    GenPublicStrings();
};

#endif /* PUBLICSTRINGS_H_*/
