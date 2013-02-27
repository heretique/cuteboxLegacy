#include <QSettings>
#include <QVariant>

#include "genappmodel.h"
#include "genfileinfomodel.h"
#include "genfilesystemmodel.h"
#include "genliterals.h"
#include "gendataengine.h"
#include "genlog.h"

GenAppModel::GenAppModel(QObject *parent) :
    QObject(parent),
    _settings(new QSettings(this)),
     _dbEngine(new GenDataEngine(this)),
    _dropboxFileInfoModel(new GenFileInfoModel(*this, this)),
    _localFileSystemModel(new GenFileSystemModel(this)),
    _currentDropboxPath(""),
    _currentRequest(WSReqNone),
    _autoGetThumbs(true),
    _autoDownload(true)
{
    if (_settings->contains(SETTINGS_AUTOGETTHUMBS))
    {
        _autoGetThumbs = settingValue(SETTINGS_AUTOGETTHUMBS).toBool();
    }
    if (_settings->contains(SETTINGS_AUTODOWNLOAD))
    {
        _autoDownload = settingValue(SETTINGS_AUTODOWNLOAD).toBool();
    }
    _dbEngine->init();
}

GenAppModel::~GenAppModel()
{
}

void GenAppModel::setSettingValue(const QString &key, const QVariant &value)
{
    _settings->setValue(key, value);
    handleModelChanged();
    emit modelChanged(*this);
}

QVariant GenAppModel::settingValue(const QString &key) const
{
    return _settings->value(key);
}

GenDataEngine &GenAppModel::dbEngine()
{
    return *_dbEngine;
}

void GenAppModel::setFileInfoList(QList<GenFileInfo *> filesInfo)
{
    _dropboxFileInfoModel->setFilesInfo(filesInfo);
}

GenFileInfoModel *GenAppModel::dropboxFileInfoModel()
{
    return _dropboxFileInfoModel;
}

GenFileSystemModel *GenAppModel::localFileSystemModel()
{
    return _localFileSystemModel;
}

void GenAppModel::setFileInfoModel(GenFileInfoModel *model)
{
    delete _dropboxFileInfoModel;
    _dropboxFileInfoModel = model;
}

void GenAppModel::setCurrentDropboxPath(const QString &currentPath)
{
    _currentDropboxPath = currentPath;
    QLOG_DEBUG("CURRENT PATH = " << _currentDropboxPath);
}

const QString &GenAppModel::currentDropboxPath() const
{
    return _currentDropboxPath;
}

void GenAppModel::setCurrentDropboxModelIndex(const QModelIndex &index)
{
    _currentDropboxModelIndex = index;
}

const QPersistentModelIndex &GenAppModel::currentDropboxModelIndex() const
{
    return _currentDropboxModelIndex;
}

void GenAppModel::setCurrentLocalPath(const QString &currentPath)
{
    _currentLocalPath = currentPath;
    QLOG_DEBUG("CURRENT LOCAL PATH = " << _currentLocalPath);
}

const QString &GenAppModel::currentLocalPath() const
{
    return _currentLocalPath;
}

void GenAppModel::setCurrentLocalModelIndex(const QModelIndex &index)
{
    _currentLocalModelIndex = index;
}

const QPersistentModelIndex &GenAppModel::currentLocalModelIndex() const
{
    return _currentLocalModelIndex;
}

const GenFileInfo *GenAppModel::currentFileInfo()
{
    return dropboxFileInfoModel()->fileInfo(
                currentDropboxModelIndex());
}


void GenAppModel::setCurrentRequest(const GenDropboxWSRequestIDs currentRequest)
{
    _currentRequest = currentRequest;
}

const GenDropboxWSRequestIDs GenAppModel::currentRequest() const
{
    return _currentRequest;
}

bool GenAppModel::autoGetTumbs()
{
    return _autoGetThumbs;
}

bool GenAppModel::autoDownload()
{
    return _autoDownload;
}

void GenAppModel::setCurrentSelection(FilesList currentSelection)
{
    _currentSelection = currentSelection;
}

FilesList GenAppModel::currentSelection()
{
    return _currentSelection;
}

void GenAppModel::clearCurrentSelection()
{
    _currentSelection.clear();
}

void GenAppModel::setNewFolderName(QString folderName)
{
    _newFolderName = folderName;
}

const QString GenAppModel::newFolderName() const
{
    return _newFolderName;
}

void GenAppModel::setAccountInfo(GenAccountInfo accountInfo)
{
    setSettingValue(SETTING_UID, accountInfo._uid);
    setSettingValue(SETTING_DISPLAY_NAME, accountInfo._displayName);
    setSettingValue(SETTING_COUNTRY, accountInfo._country);
    setSettingValue(SETTING_SPACE_LIMIT, accountInfo._spaceLimit);
    setSettingValue(SETTING_USED_SPACE, accountInfo._usedSpace);
    setSettingValue(SETTING_SHARED_SPACE, accountInfo._sharedSpace);
    handleModelChanged();
    emit modelChanged(*this);
}

GenAccountInfo GenAppModel::getAccountInfo() const
{
    GenAccountInfo accountInfo;
    accountInfo._uid = settingValue(SETTING_UID).toString();
    accountInfo._country = settingValue(SETTING_COUNTRY).toString();
    accountInfo._displayName = settingValue(SETTING_DISPLAY_NAME).toString();
    accountInfo._spaceLimit = settingValue(SETTING_SPACE_LIMIT).toULongLong();
    accountInfo._usedSpace = settingValue(SETTING_USED_SPACE).toULongLong();
    accountInfo._sharedSpace = settingValue(SETTING_SHARED_SPACE).toULongLong();
    return accountInfo;
}

bool GenAppModel::isFirstRun()
{
    QVariant result = settingValue(SETTING_FIRST_RUN);
    if (result.isValid())
        return result.toBool();

    return true;
}

void GenAppModel::handleModelChanged()
{
    if (_settings->contains(SETTINGS_AUTOGETTHUMBS))
    {
        _autoGetThumbs = settingValue(SETTINGS_AUTOGETTHUMBS).toBool();
    }
    if (_settings->contains(SETTINGS_AUTODOWNLOAD))
    {
        _autoDownload = settingValue(SETTINGS_AUTODOWNLOAD).toBool();
    }
}
