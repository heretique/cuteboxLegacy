#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QScrollBar>

#include "genmainview.h"
#include "ui_genmainview.h"
#include "genlistview.h"
#include "genappmodel.h"
#include "genfileinfomodel.h"
#include "genconstants.h"
#include "genfileinfo.h"
#include "genmainwindow.h"
#include "genfileinfodelegate.h"
#include "genresourcereader.h"
#include "genmenubar.h"
#include "genliterals.h"
#include "genpublicstrings.h"
#include "genquerypopup.h"
#include "genmediagallery.h"
#include "genstatuswidget.h"
#include "genwsengine.h"
#include "gendataengine.h"
#include "genfilebrowserview.h"
#include "genthumbnailmanager.h"
#include "genpopupmenu.h"
#include "genutils.h"
#include "genlog.h"

GenMainView::GenMainView(GenWSEngine *wsEngine, GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL),
    _wsEngine(wsEngine),
    _newKinetiArea(NULL),
    _newList(NULL),
    _newModel(NULL),
    _animInProgress(false),
    _state(StateListView),
    _selectionCount(0),
    _isForward(true),
    _thumbnailManager(NULL)
{
}

GenMainView::~GenMainView()
{
    delete ui;
    ui = 0;
    if (_thumbnailManager)
    {
        _thumbnailManager->exit();
        _thumbnailManager->wait();
        delete _thumbnailManager;
        _thumbnailManager = NULL;
    }
}

void GenMainView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    _thumbnailManager = new GenThumbnailManager(_appModel);
    _thumbnailManager->start();
    _mainWindow->useImageBackground(false);
    ui = new Ui::GenMainView;
    ui->setupUi(this);
    ui->kineticArea->enableKineticScrollingFor(ui->list, Qt::Vertical);
    ui->list->setModel(_appModel->dropboxFileInfoModel());
    connectModelSlots(_appModel->dropboxFileInfoModel());
    _appModel->dropboxFileInfoModel()->startThumbnailGeneration();
    ui->list->setSpacing(5);
    ui->list->setItemDelegate(new GenFileInfoDelegate());
    ui->list->verticalScrollBar()->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {\n"
                                                                   "    background: grey;\n"
                                                                   "    width: 6px;\n"
                                                                   "    }\n"
                                                                   "    QScrollBar::handle:vertical {\n"
                                                                   "    background: white;\n"
                                                                   "    min-height: 20px; "
                                                                   "    }\n"));
    ui->list->verticalScrollBar()->hide();
    connect(ui->list , SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
    connect(ui->kineticArea->kineticEngine(), SIGNAL(flicked(GenKineticScrollEngine::SwipeDirection)),
            SLOT(handleFlicked(GenKineticScrollEngine::SwipeDirection)));
    connectSlots();
    _mainWindow->statusBar()->showExitButton(true);
    GenMainView::State state = StateListView;
    if (data.isValid())
        state = (GenMainView::State)data.toInt();
    _state = state;
    if (StateListView != _state)
    {
        _mainWindow->statusBar()->showHomeButton("dropbox" != _appModel->currentDropboxPath());
        _mainWindow->statusBar()->setText(_appModel->currentDropboxPath());
        if (_state == StateSkipUpdate)
            setState(StateListView);
        return;
    }
    _appModel->setCurrentDropboxPath("dropbox");
    _mainWindow->statusBar()->showHomeButton(false);
    listFiles("dropbox", true);
}

void GenMainView::viewActivated()
{
    ui->list->verticalScrollBar()->show();
}

void GenMainView::handleItemClicked(const QModelIndex &index)
{
    if (StateDownloadSelection == _state || StateFileActionSelection == _state)
    {
        handleItemSelected(index);
    }
    else
    {
        listItem(index);
    }
}

void GenMainView::handleItemSelected(const QModelIndex &index)
{
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    const GenFileInfo *fileInfo = _appModel->dropboxFileInfoModel()->fileInfo(index);
    if (fileInfo && fileInfo->_isDir && _state == StateDownloadSelection)
    {
        selectionModel->select(index, QItemSelectionModel::Deselect |
                               QItemSelectionModel::Current |
                               QItemSelectionModel::Rows);
    }
    int count = selectionModel->selectedIndexes().size();
    if (StateDownloadSelection == _state)
    {
        if (count == 1 && _selectionCount == 0)
        {
            _mainWindow->queryPopup()->
                    setText(GenPublicStrings::STR_CONFIRM_DOWNLOAD_FILES.arg(QString::number(count)));
            setOKCancelMenuButtons();
        }
        else if (count == 0 && _selectionCount == 1)
        {
            _mainWindow->queryPopup()->
                    setText(GenPublicStrings::STR_CHOOSE_DOWNLOAD_FILES);
            setCancelMenuButton(*(_mainWindow->menuBar()));
        }
        else if(count > 0)
        {
            _mainWindow->queryPopup()->
                    setText(GenPublicStrings::STR_CONFIRM_DOWNLOAD_FILES.arg(QString::number(count)));
        }
    }
    _selectionCount = count;
    if (StateFileActionSelection == _state)
    {
        if (count > 0)
            _mainWindow->menuBar()->enableButtons(GenMenuBar::GenMenuAllButtons);
        else
            _mainWindow->menuBar()->disableButtons(GenMenuBar::GenMenuButton1 |
                                                   GenMenuBar::GenMenuButton2 |
                                                   GenMenuBar::GenMenuButton3);
    }
}

void GenMainView::listItem(const QModelIndex &index)
{
    QString path;
    const GenFileInfo *fileInfo = _appModel->dropboxFileInfoModel()->fileInfo(index);
    if (fileInfo)
    {
        if (fileInfo->_isDir)
        {
            path = fileInfo->_path;
            _appModel->setCurrentDropboxPath(path);
            listFiles(path, true);
        }
        else
        {
            if (index.isValid() && StateListView == _state)
            {
                _appModel->setCurrentDropboxModelIndex(index);
                handleAction(ActionDROPBOXFILEDETAILS);
            }
        }
    }
}

void GenMainView::deactivate()
{
    _thumbnailManager->exit();
    _thumbnailManager->wait();
    delete _thumbnailManager;
    _thumbnailManager = NULL;
    delete ui->kineticArea;
    ui->kineticArea = NULL;
    delete ui;
    ui = NULL;
}

void GenMainView::updateMenuBar(GenMenuBar &menuBar)
{
    ActionId action = ActionDOWNLOADFILES;
    QPixmap icon = GenResourceReader::pixmapFromResource("download_icon.png");

    menuBar.setMenuActions(ActionADDNEW,
                           action,
                           ActionFILEACTIONS,
                           ActionSETTINGS);
    menuBar.setMenuIcons(GenResourceReader::pixmapFromResource("add_new_icon.png"),
                         icon,
                         GenResourceReader::pixmapFromResource("copy_icon.png"),
                         GenResourceReader::pixmapFromResource("settings_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton1 |
                           GenMenuBar::GenMenuButton2 |
                           GenMenuBar::GenMenuButton3 |
                           GenMenuBar::GenMenuButton4);
}


void GenMainView::setOKCancelMenuButtons()
{

    switch(_state)
    {
    case StateDownloadSelection:
        _mainWindow->setQueryAction(ActionSTARTDOWNLOADFILES);
        break;
    case StateUploadSelection:
        _mainWindow->setQueryAction(ActionSTARTUPLOADFILES);
        break;
    case StateCopySelection:
        _mainWindow->setQueryAction(ActionSTARTCOPYFILES);
        break;
    case StateMoveSelection:
        _mainWindow->setQueryAction(ActionSTARTMOVEFILES);
        break;
    case StateDeleteSelection:
        _mainWindow->setQueryAction(ActionSTARTDELETEFILES);
        break;
    default:
        _mainWindow->setQueryAction(ActionOK);
    }

    _mainWindow->menuBar()->setMenuActions(ActionOK,
                                           ActionNONE,
                                           ActionNONE,
                                           ActionCANCEL);
    _mainWindow->menuBar()->setMenuIcons(GenResourceReader::pixmapFromResource("ok_icon.png"),
                                         QPixmap(),
                                         QPixmap(),
                                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    _mainWindow->menuBar()->setMenuButtons(GenMenuBar::GenMenuButton1 |
                                           GenMenuBar::GenMenuButton4,
                                           GenMenuBar::GenMenuButton1 |
                                           GenMenuBar::GenMenuButton4);
}

void GenMainView::setFileActionsButtons()
{
    _mainWindow->menuBar()->setMenuActions(ActionCOPYFILES,
                                           ActionMOVEFILES,
                                           ActionDELETEFILES,
                                           ActionCANCEL);
    _mainWindow->menuBar()->setMenuIcons(GenResourceReader::pixmapFromResource("copy_icon.png"),
                                         GenResourceReader::pixmapFromResource("move_icon.png"),
                                         GenResourceReader::pixmapFromResource("delete_icon.png"),
                                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    _mainWindow->menuBar()->setMenuButtons(GenMenuBar::GenMenuButton1 |
                                           GenMenuBar::GenMenuButton2 |
                                           GenMenuBar::GenMenuButton3 |
                                           GenMenuBar::GenMenuButton4,
                                           GenMenuBar::GenMenuButton4);
}

void GenMainView::getSelection()
{
    _appModel->clearCurrentSelection();
    FilesList selection;
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    QModelIndexList indexes = selectionModel->selection().indexes();
    QListIterator<QModelIndex> it (indexes);
    QModelIndex index;
    while(it.hasNext())
    {
        index = it.next();
        const GenFileInfo *fileInfo = _appModel->dropboxFileInfoModel()->fileInfo(index);
        if (fileInfo)
            selection.insert(fileInfo->_path, fileInfo->_modifiedTime);
    }
#ifdef Q_LOGGING
    QHashIterator<QString, QDateTime> i(selection);
    QLOG_DEBUG("SELECTION:");
    while(i.hasNext())
    {
        i.next();
        QString path = i.key();
        QDateTime lastModified = i.value();
        QLOG_DEBUG(path << ", " << lastModified);
    }
#endif
    _selectionCount = selection.size();
    _appModel->setCurrentSelection(selection);
}

void GenMainView::connectSlots()
{
    connect(_wsEngine, SIGNAL(metaDataReceived(GenFileInfo*, QList<GenFileInfo*>)),
            SLOT(handleFileInfoReceived(GenFileInfo*,QList<GenFileInfo*>)));
    connect(_wsEngine, SIGNAL(metaDataNotModified()),
            SLOT(handleMetaDataNotModified()));
    connect(_wsEngine, SIGNAL(filesDeleted()), SLOT(refreshView()));
    connect(_wsEngine, SIGNAL(filesCopiedOrMoved()), SLOT(refreshView()));
    connect(_wsEngine, SIGNAL(uploadFinished()), SLOT(handleUploadFinished()));
    connect(_wsEngine, SIGNAL(folderCreated(GenFileInfo*)), SLOT(handleFolderCreated(GenFileInfo*)));
}

void GenMainView::disconnectSlots()
{
    disconnect(_wsEngine, 0, this, 0);
}

void GenMainView::aboutToDeactivate()
{
    ui->list->verticalScrollBar()->hide();
    disconnectSlots();
    disconnectModelSlots();
}

void GenMainView::connectModelSlots(GenFileInfoModel *model)
{
    connect(model,
            SIGNAL(requestThumbnail(QString)),
            _thumbnailManager,
            SLOT(handleThumbnailRequest(QString)), Qt::QueuedConnection);
    connect(_thumbnailManager,
            SIGNAL(thumbnailError(QString,QNetworkReply::NetworkError,int,QByteArray)),
            model,
            SLOT(handleThumbnailError(QString,QNetworkReply::NetworkError,int,QByteArray)), Qt::QueuedConnection);
    connect(_thumbnailManager,
            SIGNAL(thumbnailReady(QString,QImage)),
            model,
            SLOT(handleThumbnailReady(QString,QImage)), Qt::QueuedConnection);
}

void GenMainView::uploadMediaFiles(GenMediaGallery::MediaFileTypes mediaType)
{
    QLOG_DEBUG("GenMainView::uploadMediaFiles - START");
    updateMenuBar(*(_mainWindow->menuBar()));
    GenMediaGallery *gallery = new GenMediaGallery;
    QStringList pictures;
    switch (mediaType)
    {
    case (GenMediaGallery::ImageFile):
        _mainWindow->setWindowTitle("Select Images");
        break;
    case (GenMediaGallery::VideoFile):
        _mainWindow->setWindowTitle("Select Videos");
        break;
    case (GenMediaGallery::AudioFile):
        _mainWindow->setWindowTitle("Select Audio");
        break;
    }
#ifdef Q_OS_SYMBIAN
    _mainWindow->showMaximized();
#endif // Q_OS_SYMBIAN
    gallery->getMediaFiles(pictures, mediaType, true);
#ifdef Q_OS_SYMBIAN
    _mainWindow->showFullScreen();
#endif // Q_OS_SYMBIAN
    delete gallery;
    if (pictures.size())
    {
        FilesList selection;
        QStringListIterator i(pictures);
        while (i.hasNext())
        {
            selection.insert(i.next(), QDateTime());
        }
        _appModel->setCurrentSelection(selection);
        setState(StateUploadSelection);
        setOKCancelMenuButtons();
        _mainWindow->showQueryPopup(
                    GenPublicStrings::STR_CONFIRM_UPLOAD_FILES.arg(
                        QString::number(_appModel->currentSelection().size()),
                        _appModel->currentDropboxPath()));
    }
    QLOG_DEBUG("GenMainView::uploadMediaFiles - END");
}

void GenMainView::disconnectModelSlots()
{
    _thumbnailManager->disconnect();
}

bool GenMainView::handleAction(const ActionId actionId)
{
    switch(actionId)
    {
    case ActionBACK:
        handleBack();
        return true;
    case ActionSETTINGS:
        _mainWindow->activateView("GenSettingsView",
                                  "GenMainView",
                                  true,
                                  GenFromTop);
        return true;
    case ActionDROPBOXFILEDETAILS:
        activateView("GenFileDetailsView");
        return true;
    case ActionDOWNLOADFILES:
        if (!_mainWindow->popupOpened())
        {
            setState(StateDownloadSelection);
            _mainWindow->showQueryPopup(GenPublicStrings::STR_CHOOSE_DOWNLOAD_FILES);
            setCancelMenuButton(*(_mainWindow->menuBar()));
        }
        else
        {
            _mainWindow->showNotificationInfo(GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS);
        }
        return true;
    case ActionGOTOROOT:
        _appModel->setCurrentDropboxPath("dropbox");
        listFiles("dropbox", false);
        return true;
    case ActionFILEACTIONS:
        if (!_mainWindow->popupOpened())
        {
            setState(StateFileActionSelection);
            _mainWindow->showQueryPopup(GenPublicStrings::STR_CHOOSE_FILES);
            setFileActionsButtons();
        }
        else
        {
            _mainWindow->showNotificationInfo(GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS);
        }
        return true;
    case ActionCOPYFILES:
        getSelection();
        setState(StateCopySelection);
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_COPY_FILES.arg(
                                      QString::number(_selectionCount), _appModel->currentDropboxPath()));
        setOKCancelMenuButtons();
        return true;
    case ActionMOVEFILES:
        getSelection();
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_MOVE_FILES.arg(
                                      QString::number(_selectionCount), _appModel->currentDropboxPath()));
        setState(StateMoveSelection);
        setOKCancelMenuButtons();
        return true;
    case ActionDELETEFILES:
        getSelection();
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_DELETE_FILES.arg(
                                      QString::number(_selectionCount)));
        setState(StateDeleteSelection);
        setOKCancelMenuButtons();
        return true;
    case ActionADDNEWAUDIO:
        uploadMediaFiles(GenMediaGallery::AudioFile);
        return true;
    case ActionADDNEWVIDEO:
        uploadMediaFiles(GenMediaGallery::VideoFile);
        return true;
    case ActionADDNEWPICTURE:
        uploadMediaFiles(GenMediaGallery::ImageFile);
        return true;
    case ActionADDNEW:
        if (!_mainWindow->popupOpened())
        {
            QList<QPair<ActionId, QString> > actions;
            actions.append(qMakePair<ActionId, QString>(ActionADDNEWFOLDER, "Folder"));
            actions.append(qMakePair<ActionId, QString>(ActionADDNEWAUDIO, "Audio clip"));
            actions.append(qMakePair<ActionId, QString>(ActionADDNEWPICTURE, "Pictures"));
            actions.append(qMakePair<ActionId, QString>(ActionADDNEWVIDEO, "Videos"));
            actions.append(qMakePair<ActionId, QString>(ActionADDNEWLOCALFILE, "Local files"));
            _mainWindow->showPopupMenu("Add new:", actions);
        }
        else
        {
            _mainWindow->showNotificationInfo(GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS);
        }
        return true;
    case ActionADDNEWLOCALFILE:
        _mainWindow->activateView("GenFileBrowserView",
                                  GenFileBrowserView::StateUploadSelection,
                                  true,
                                  GenFromBottom);
        return true;
    case ActionADDNEWFOLDER:
        _mainWindow->showFilenamePopup();
        return true;
    case ActionSTARTDOWNLOADFILES:
        getSelection();
        setState(StateListView);
        handleDownloadFiles();
        return true;
    case ActionSTARTCOPYFILES:
        setState(StateListView);
        handleCopyFiles();
        break;
    case ActionSTARTMOVEFILES:
        setState(StateListView);
        handleMoveFiles();
        break;
    case ActionSTARTDELETEFILES:
        setState(StateListView);
        handleDeleteFiles();
        break;
    case ActionSTARTUPLOADFILES:
        setState(StateListView);
        handleUploadFiles();
        break;
    case ActionCREATEFOLDER:
        handleCreateFolder();
        break;
    case ActionOK:
    case ActionCANCEL:
        setState(StateListView);
        return true;
    default:
        break;
    }

    return false;
}

void GenMainView::resizeEvent(QResizeEvent *e)
{
    if (ui)
    {
        ui->kineticArea->resize(e->size());
        ui->list->resize(e->size());
    }
    QWidget::resizeEvent(e);
}

void GenMainView::changeFileInfoModel(GenFileInfoModel *newModel, bool isForward)
{
    if (_animInProgress || !ui)
        return;

    _mainWindow->statusBar()->showHomeButton("dropbox" != _appModel->currentDropboxPath());
    ui->list->verticalScrollBar()->hide();
    ui->kineticArea->enableCache();
    createNewList();
    _newList->setModel(newModel);
    _newKinetiArea->enableCache();
    _newModel = newModel;

    QAnimationGroup* aniGroup = _mainWindow->createViewAnimation(*(ui->kineticArea), *_newKinetiArea,
                                                isForward ? GenFromRight : GenFromLeft);
    connect(aniGroup, SIGNAL( finished() ), this,
            SLOT( changeAnimationFinished()));
    aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
    _animInProgress = true;
    if (StateCopySelection == _state)
    {
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_COPY_FILES.arg(
                                      QString::number(_selectionCount), _appModel->currentDropboxPath()));
    }
    else if (StateMoveSelection == _state)
    {
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_MOVE_FILES.arg(
                                      QString::number(_selectionCount), _appModel->currentDropboxPath()));
    }
    else if (StateUploadSelection == _state)
    {
        _mainWindow->setQueryText(GenPublicStrings::STR_CONFIRM_UPLOAD_FILES.arg(
                                      QString::number(_appModel->currentSelection().size()),
                                      _appModel->currentDropboxPath()));
    }
}

void GenMainView::setState(State state)
{
    _state = state;
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    selectionModel->clearSelection();
    if (StateDownloadSelection == _state || StateFileActionSelection == _state)
    {
        _selectionCount = 0;
        ui->list->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else
    {
        ui->list->setSelectionMode(QAbstractItemView::NoSelection);
    }
}

void GenMainView::createNewList()
{
    delete _newKinetiArea;
    delete _newList;
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);
    _newKinetiArea = new GenKineticScrollWidget(this);
    _newKinetiArea->setObjectName(QString::fromUtf8("kineticArea"));
    _newKinetiArea->setGeometry(QRect(0, 0, 360, 490));
    sizePolicy.setHeightForWidth(_newKinetiArea->sizePolicy().hasHeightForWidth());
    _newKinetiArea->setSizePolicy(sizePolicy);
    _newKinetiArea->setMinimumSize(QSize(20, 20));
    _newKinetiArea->setContextMenuPolicy(Qt::NoContextMenu);
    _newList = new GenListView(_newKinetiArea);
    _newList->setObjectName(QString::fromUtf8("list"));
    _newList->setGeometry(ui->list->geometry());
    sizePolicy.setHeightForWidth(_newList->sizePolicy().hasHeightForWidth());
    _newList->setSizePolicy(sizePolicy);
    _newList->setFocusPolicy(Qt::NoFocus);
    _newList->setContextMenuPolicy(Qt::NoContextMenu);
    _newKinetiArea->enableKineticScrollingFor(_newList, Qt::Vertical);
    _newList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _newList->setStyleSheet(QString::fromUtf8("QListView\n"
                                              "{\n"
                                              "	color: rgb(92, 95, 97);\n"
                                              "    background-color: rgb(238, 238, 238);\n"
                                              "    selection-background-color:  qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,"
                                              "    stop: 0 rgb(173, 173, 173, 255), stop: 1 rgb(238, 238, 238, 255));\n"
                                              "}"));
    _newList->setItemDelegate(new GenFileInfoDelegate());
    _newList->setFrameShape(QFrame::NoFrame);
    _newList->setFrameShadow(QFrame::Plain);
    _newList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _newList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _newList->setSpacing(5);
    _newList->verticalScrollBar()->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {\n"
                                                                   "    background: grey;\n"
                                                                   "    width: 6px;\n"
                                                                   "    }\n"
                                                                   "    QScrollBar::handle:vertical {\n"
                                                                   "    background: white;\n"
                                                                   "    min-height: 20px; "
                                                                   "    }\n"));
    _newList->verticalScrollBar()->hide();
}

void GenMainView::changeAnimationFinished()
{
    QLOG_DEBUG("GenMainView::changeAnimationFinished - _newList geom = " << _newKinetiArea->geometry());
    delete ui->kineticArea;
    ui->kineticArea = _newKinetiArea;
    ui->list = _newList;
    _newKinetiArea->disableCache();
    _newKinetiArea->setEnabled(true);
    _newList->verticalScrollBar()->show();
    connect(ui->list , SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
    connect(ui->kineticArea->kineticEngine(), SIGNAL(flicked(GenKineticScrollEngine::SwipeDirection)),
            SLOT(handleFlicked(GenKineticScrollEngine::SwipeDirection)));
    _animInProgress = false;
    _appModel->setFileInfoModel(_newModel);
    connectModelSlots(_newModel);
    _newModel->startThumbnailGeneration();
    _newModel = NULL;
    _newKinetiArea = NULL;
    _newList = NULL;
}

void GenMainView::handleBack()
{
    if (StateDownloadSelection != _state && StateFileActionSelection != _state)
    {
        QString currentPath = _appModel->currentDropboxPath();
        currentPath = currentPath.left(currentPath.lastIndexOf('/'));
        if (currentPath != _appModel->currentDropboxPath())
        {
            _appModel->setCurrentDropboxPath(currentPath);
            listFiles(currentPath, false);
        }
    }
}

void GenMainView::handleFlicked(GenKineticScrollEngine::SwipeDirection direction)
{
    if (GenKineticScrollEngine::Right == direction)
        handleBack();
}

void GenMainView::handleFileInfoReceived(GenFileInfo *parent, QList<GenFileInfo *> content)
{
    _appModel->dbEngine().replaceOrAddFileInfo(parent);
    _appModel->dbEngine().syncFileInfo(parent->_path, content);
    _mainWindow->statusBar()->setText(parent->_path);
    delete parent;
    GenFileInfoModel *newModel = new GenFileInfoModel(*_appModel);
    newModel->setFilesInfo(content);

#ifdef FEATURE_PHOTO_WALL
    if (_appModel->currentDropboxPath().startsWith("dropbox/Photos"))
    {
        _appModel->setFileInfoModel(newModel);
        activateView("GenPhotoView");
        return;
    }
#endif // FEATURE_PHOTO_WALL

    changeFileInfoModel(newModel, _isForward);
}

void GenMainView::handleMetaDataNotModified()
{
    QList<GenFileInfo*> content = _appModel->dbEngine().retrieveFilesInfo(
                _appModel->currentDropboxPath());
    _mainWindow->statusBar()->setText(_appModel->currentDropboxPath());
    for (int i = 0; i < content.size(); ++i)
    {
        content.at(i)->_icon = GenResourceReader::iconFromResource(content.at(i)->_iconName);
    }
    GenFileInfoModel *newModel = new GenFileInfoModel(*_appModel);
    newModel->setFilesInfo(content);

#ifdef FEATURE_PHOTO_WALL
    if (_appModel->currentDropboxPath().startsWith("dropbox/Photos"))
    {
        _appModel->setFileInfoModel(newModel);
        activateView("GenPhotoView");
        return;
    }
#endif // FEATURE_PHOTO_WALL

    changeFileInfoModel(newModel, _isForward);
}

void GenMainView::listFiles(const QString &path, bool isForward)
{
    QLOG_DEBUG("GenApplicationManager::listFiles - path = " << path );
    _isForward = isForward;
    GenFileInfo *parent = _appModel->dbEngine().retrieveFileInfo(path);
    if (parent && !parent->_hash.isEmpty())
    {
        QList<QPair<QString, QString> > params;
        params.append(qMakePair<QString, QString> ("hash", parent->_hash));
        QLOG_DEBUG("GenApplicationManager::listFiles - hash = " << parent->_hash);
        _wsEngine->startWSRequest(GetDropboxRequestById(WSReqGetMetadata), path, params, QByteArray());
    }
    else
    {
        _wsEngine->startWSRequest(GetDropboxRequestById(WSReqGetMetadata), path, QByteArray());
    }
    delete parent;
    _mainWindow->showWaitingIndicator("Loading...");
}

void GenMainView::refreshView()
{
    listFiles(_appModel->currentDropboxPath(), true);
}

void GenMainView::handleDownloadFiles()
{
    _wsEngine->downloadFiles(_appModel->currentSelection());
}

void GenMainView::handleCopyFiles()
{
    _wsEngine->copyOrMoveFiles(_appModel->currentSelection(),
                               _appModel->currentDropboxPath());
    _mainWindow->showWaitingIndicator("Copying files...");
}

void GenMainView::handleMoveFiles()
{
    _wsEngine->copyOrMoveFiles(_appModel->currentSelection(),
                               _appModel->currentDropboxPath(),
                               true);
    _mainWindow->showWaitingIndicator("Moving files...");
}

void GenMainView::handleDeleteFiles()
{
    _wsEngine->deleteFiles(_appModel->currentSelection());
    _mainWindow->showWaitingIndicator("Deleting files...");
}

void GenMainView::handleUploadFiles()
{
    _uploadPath = _appModel->currentDropboxPath();
    _wsEngine->uploadFiles(_appModel->currentDropboxPath(),
                           _appModel->currentSelection());
}

void GenMainView::handleUploadFinished()
{
    if (_uploadPath == _appModel->currentDropboxPath())
    {
        refreshView();
    }
    _uploadPath.clear();
}

void GenMainView::handleFolderCreated(GenFileInfo *fileInfo)
{
    _appModel->dbEngine().replaceOrAddFileInfo(fileInfo);
    _appModel->dropboxFileInfoModel()->addFileInfo(fileInfo);
}

void GenMainView::handleCreateFolder()
{
    QString destination = _appModel->currentDropboxPath();
    if (destination == "dropbox")
        destination = _appModel->newFolderName();
    else
        destination = destination.mid(destination.indexOf("/") + 1) + "/" + _appModel->newFolderName();
    _wsEngine->createFolder(destination);
}
