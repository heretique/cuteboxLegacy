#ifndef _GENAPPMODEL_H_
#define _GENAPPMODEL_H_

#include <QObject>
#include <QPersistentModelIndex>
#include <QStringList>
#include "gendropboxrequests.h"
#include "gentypes.h"

class QSettings;
class GenFileInfo;
class GenFileInfoModel;
class GenFileSystemModel;
class GenDataEngine;

class GenAppModel : public QObject
{
Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent, a QObject* argument. The model's parent.
     */
    GenAppModel(QObject *parent = NULL);

    /*!
     * \brief Destructor
     */
    ~GenAppModel();

    void setSettingValue(const QString &key, const QVariant &value);

    QVariant settingValue(const QString &key) const;

    GenDataEngine &dbEngine();

    void setFileInfoList(QList<GenFileInfo*> filesInfo);

    GenFileInfoModel *dropboxFileInfoModel();

    GenFileSystemModel *localFileSystemModel();

    void setFileInfoModel(GenFileInfoModel *model);

    void setCurrentDropboxPath(const QString &currentDropboxPath);
    const QString &currentDropboxPath() const;

    void setCurrentDropboxModelIndex(const QModelIndex& index);
    const QPersistentModelIndex &currentDropboxModelIndex() const;

    void setCurrentLocalPath(const QString &currentDropboxPath);
    const QString &currentLocalPath() const;

    void setCurrentLocalModelIndex(const QModelIndex& index);
    const QPersistentModelIndex &currentLocalModelIndex() const;

    const GenFileInfo *currentFileInfo();

    void setCurrentRequest(const GenDropboxWSRequestIDs currentRequest);
    const GenDropboxWSRequestIDs currentRequest() const;

    bool autoGetTumbs();
    bool autoDownload();

    void setCurrentSelection(FilesList currentSelection);
    FilesList currentSelection();
    void clearCurrentSelection();

    void setNewFolderName(QString folderName);
    const QString newFolderName() const;

    void setAccountInfo(GenAccountInfo accountInfo);
    GenAccountInfo getAccountInfo() const;

    bool isFirstRun();

protected slots:
    void handleModelChanged();

signals:
    void modelChanged(GenAppModel& model);

private:   // DATA
    QSettings *_settings;
    GenDataEngine *_dbEngine;
    GenFileInfoModel *_dropboxFileInfoModel;
    GenFileSystemModel *_localFileSystemModel;
    QString _currentDropboxPath;
    QPersistentModelIndex _currentDropboxModelIndex;
    QString _currentLocalPath;
    QPersistentModelIndex _currentLocalModelIndex;
    GenDropboxWSRequestIDs _currentRequest;
    bool _autoGetThumbs;
    bool _autoDownload;
    FilesList _currentSelection;
    QString _newFolderName;
};

#endif // _GENAPPMODEL_H_
