#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QScrollBar>

#include "genfilebrowserview.h"
#include "ui_genfilebrowserview.h"
#include "genlistview.h"
#include "genappmodel.h"
#include "genfileinfomodel.h"
#include "genconstants.h"
#include "genfileinfo.h"
#include "genmainwindow.h"
#include "genfileinfodelegate.h"
#include "genfilesystemmodel.h"
#include "genfilesystemfilterproxymodel.h"
#include "genresourcereader.h"
#include "genmenubar.h"
#include "genpublicstrings.h"
#include "genquerypopup.h"
#include "genstatuswidget.h"
#include "genmainview.h"
#include "genliterals.h"
#include "genutils.h"
#include "genlog.h"

GenFileBrowserView::GenFileBrowserView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL),
    _newKineticArea(NULL), _newList(NULL), _animInProgress(false), _selectionCount(0)
{

}

GenFileBrowserView::~GenFileBrowserView()
{
    delete ui;
}

void GenFileBrowserView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenFileBrowserView;
    ui->setupUi(this);
    ui->kineticArea->enableKineticScrollingFor(ui->list, Qt::Vertical);
    _filterModel = new GenFileSystemFilterProxyModel(this);
    _filterModel->setSourceModel(_appModel->localFileSystemModel());
    ui->list->setModel(_filterModel);
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

    GenFileBrowserView::State state = StateListView;
    if (data.isValid())
        state = (GenFileBrowserView::State)data.toInt();
    setState(state);
    if (StateSkipUpdate == _state)
    {
        _mainWindow->statusBar()->showHomeButton("" != _appModel->currentLocalPath());
        _mainWindow->statusBar()->setText(_appModel->currentLocalPath());
        setState(StateListView);
        return;
    }
    _appModel->localFileSystemModel()->setRootPath("");
    _appModel->setCurrentLocalPath("");
    _mainWindow->statusBar()->showHomeButton(false);
    _mainWindow->statusBar()->setText("My Device");
}

void GenFileBrowserView::viewActivated()
{
    if (StateUploadSelection == _state)
    {
        _mainWindow->showQueryPopup(GenPublicStrings::STR_CHOOSE_UPLOAD_FILES);
        setCancelMenuButton(*(_mainWindow->menuBar()));
    }
    else if (StateFolderSelection == _state)
    {
        _mainWindow->showQueryPopup(GenPublicStrings::STR_CHOOSE_NEW_DOWNLOAD_LOCATION);
        _mainWindow->setQueryAction(ActionDOWNLOADLOCATIONSELECTED);
        setOKCancelMenuButtons(*(_mainWindow->menuBar()));
    }
    ui->list->verticalScrollBar()->show();
}

void GenFileBrowserView::aboutToDeactivate()
{
    ui->list->verticalScrollBar()->hide();
}

void GenFileBrowserView::handleItemClicked(const QModelIndex &index)
{
    listItem(index);
}

void GenFileBrowserView::handleItemSelected(const QModelIndex &index)
{
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    int size = _appModel->localFileSystemModel()->size(index);
    if (size > FILE_SIZE_UPLOAD_LIMIT && _state == StateUploadSelection)
    {
        selectionModel->select(index, QItemSelectionModel::Deselect |
                               QItemSelectionModel::Current |
                               QItemSelectionModel::Rows);
        _mainWindow->showNotificationInfo("<b>!</b> File exceeds upload limit.");
    }
    int count = selectionModel->selectedIndexes().size();
    if (count == 1 && _selectionCount == 0)
    {
        _mainWindow->queryPopup()->
                setText(GenPublicStrings::STR_UPLOAD_SELECTED_COUNT.arg(QString::number(count)));
        setOKCancelMenuButtons(*(_mainWindow->menuBar()));
        _mainWindow->setQueryAction(ActionLOCALFILESSELECTED);
    }
    else if (count == 0 && _selectionCount == 1)
    {
        _mainWindow->queryPopup()->
                setText(GenPublicStrings::STR_CHOOSE_UPLOAD_FILES);
        setCancelMenuButton(*(_mainWindow->menuBar()));
    }
    else
    {
        _mainWindow->queryPopup()->
                setText(GenPublicStrings::STR_UPLOAD_SELECTED_COUNT.arg(QString::number(count)));
    }
    _selectionCount = count;
}

void GenFileBrowserView::listItem(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QModelIndex sourceIndex = _filterModel->mapToSource(index);
    _appModel->setCurrentLocalModelIndex(index);

    QString path;
    QFileSystemModel *model = _appModel->localFileSystemModel();
    path = model->filePath(sourceIndex);
    if (model->isDir(sourceIndex))
    {
        _appModel->setCurrentLocalPath(path);
        changeFolder(sourceIndex);
    }
    else
    {
        handleItemSelected(sourceIndex);
    }
}

void GenFileBrowserView::getSelection()
{
    _appModel->clearCurrentSelection();
    _selectionCount = 0;
    FilesList selection;
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    QModelIndexList indexes = selectionModel->selection().indexes();
    QListIterator<QModelIndex> it (indexes);
    QModelIndex index;
    QFileSystemModel *model = _appModel->localFileSystemModel();
    while(it.hasNext())
    {
        index = it.next();
        selection.insert(model->filePath(_filterModel->mapToSource(index)), QDateTime());
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
    _appModel->setCurrentSelection(selection);
}

void GenFileBrowserView::deactivate()
{
    delete ui->kineticArea;
    ui->kineticArea = NULL;
    delete _filterModel;
    _filterModel = NULL;
    delete ui;
    ui = NULL;
}

void GenFileBrowserView::updateMenuBar(GenMenuBar &menuBar)
{
}

bool GenFileBrowserView::handleAction(const ActionId actionId)
{
    switch(actionId)
    {
    case ActionBACK:
        handleBack();
        return true;
    case ActionGOTOROOT:
        _appModel->setCurrentLocalPath("");
        changeFolder(_appModel->localFileSystemModel()->index(""), false);
        return true;
    case ActionLOCALFILESSELECTED:
        getSelection();
        _mainWindow->showQueryPopup(
                    GenPublicStrings::STR_CONFIRM_UPLOAD_FILES.arg(
                        QString::number(_appModel->currentSelection().size()),
                        _appModel->currentDropboxPath()));
        setOKCancelMenuButtons(*(_mainWindow->menuBar()));
        _mainWindow->setQueryAction(ActionSTARTUPLOADFILES);
        activateView("GenMainView", (int)GenMainView::StateUploadSelection, true, GenFromTop);
    case ActionCANCEL:
        _selectionCount = 0;
        if (StateUploadSelection == _state)
            activateView("GenMainView", (int)GenMainView::StateSkipUpdate, true, GenFromTop);
        else if (StateFolderSelection)
            activateView("GenSettingsView", QVariant(), true, GenFromLeft);
        break;
    case ActionDOWNLOADLOCATIONSELECTED:
        _appModel->setSettingValue(SETTINGS_DOWNLOAD_LOCATION, _appModel->currentLocalPath());
        activateView("GenSettingsView", QVariant(), true, GenFromLeft);
        return true;
    default:
        break;
    }

    return false;
}

void GenFileBrowserView::resizeEvent(QResizeEvent *e)
{
    if (ui)
    {
        ui->kineticArea->resize(e->size());
        ui->list->resize(e->size());
    }
    QWidget::resizeEvent(e);
}

QWidget *GenFileBrowserView::primaryWidget()
{
    if (ui)
        return ui->kineticArea;
    return NULL;
}

void GenFileBrowserView::changeFolder(const QModelIndex &index, bool isForward)
{
    if (_animInProgress || !ui)
        return;

    _mainWindow->statusBar()->showHomeButton("" != _appModel->currentLocalPath());
    if (!_appModel->currentLocalPath().isEmpty())
        _mainWindow->statusBar()->setText(_appModel->currentLocalPath());
    else
        _mainWindow->statusBar()->setText("My Device");

    ui->list->verticalScrollBar()->hide();
    ui->kineticArea->enableCache();
    createNewList();
    _filterModel->setSourceModel(_appModel->localFileSystemModel());
    _newList->setModel(_filterModel);
    _newList->setRootIndex(_filterModel->mapFromSource(index));
    _newKineticArea->enableCache();

    QAnimationGroup* aniGroup = _mainWindow->createViewAnimation(*(ui->kineticArea), *_newKineticArea,
                                                isForward ? GenFromRight : GenFromLeft);
    connect(aniGroup, SIGNAL( finished() ), this,
            SLOT( changeAnimationFinished()));
    aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
    _animInProgress = true;
}

void GenFileBrowserView::createNewList()
{
    delete _newKineticArea;
    delete _newList;
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);
    _newKineticArea = new GenKineticScrollWidget(this);
    _newKineticArea->setObjectName(QString::fromUtf8("kineticArea"));
    _newKineticArea->setGeometry(QRect(0, 0, 360, 490));
    sizePolicy.setHeightForWidth(_newKineticArea->sizePolicy().hasHeightForWidth());
    _newKineticArea->setSizePolicy(sizePolicy);
    _newKineticArea->setMinimumSize(QSize(20, 20));
    _newKineticArea->setContextMenuPolicy(Qt::NoContextMenu);
    _newList = new GenListView(_newKineticArea);
    _newList->setObjectName(QString::fromUtf8("list"));
    _newList->setGeometry(ui->list->geometry());
    sizePolicy.setHeightForWidth(_newList->sizePolicy().hasHeightForWidth());
    _newList->setSizePolicy(sizePolicy);
    _newList->setFocusPolicy(Qt::NoFocus);
    _newList->setContextMenuPolicy(Qt::NoContextMenu);
    _newKineticArea->enableKineticScrollingFor(_newList, Qt::Vertical);
    _newList->setStyleSheet(QString::fromUtf8("QListView {\n"
                                              "	color: rgb(92, 95, 97);\n"
                                              "	background-color: rgb(238, 238, 238);\n"
                                              "	selection-background-color:  qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,                                      	stop: 0 rgb(173, 173, 173, 255), stop: 1 rgb(238, 238, 238, 255));\n"
                                              "}"));
    _newList->setIconSize(QSize(48, 48));
    _newList->setItemDelegate(new GenFileInfoDelegate());
    _newList->setFrameShape(QFrame::NoFrame);
    _newList->setFrameShadow(QFrame::Plain);
    _newList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _newList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _newList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _newList->setSpacing(5);
    _newList->setSelectionMode(QAbstractItemView::MultiSelection);
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

void GenFileBrowserView::changeAnimationFinished()
{
    QLOG_DEBUG("GenFileBrowserView::changeAnimationFinished - _newList geom = " << _newKineticArea->geometry());
    delete ui->kineticArea;
    ui->kineticArea = _newKineticArea;
    ui->list = _newList;
    _newKineticArea->disableCache();
    _newKineticArea->setEnabled(true);
    _newList->verticalScrollBar()->show();
    connect(ui->list , SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemClicked(const QModelIndex&)));
    connect(ui->kineticArea->kineticEngine(), SIGNAL(flicked(GenKineticScrollEngine::SwipeDirection)),
            SLOT(handleFlicked(GenKineticScrollEngine::SwipeDirection)));
    _animInProgress = false;
    _newKineticArea = NULL;
    _newList = NULL;
}

void GenFileBrowserView::handleBack()
{
    QString currentPath = _appModel->currentLocalPath();
    int lastIndex = currentPath.lastIndexOf('/');
    if (lastIndex != -1)
        currentPath = currentPath.left(lastIndex);
    else
        currentPath = "";

    if (currentPath != _appModel->currentLocalPath())
    {
        _appModel->setCurrentLocalPath(currentPath);
        QModelIndex index = _appModel->localFileSystemModel()->index(currentPath);
        changeFolder(index, false);
    }
}

void GenFileBrowserView::handleFlicked(GenKineticScrollEngine::SwipeDirection direction)
{
    if (GenKineticScrollEngine::Right == direction)
        handleBack();
}

void GenFileBrowserView::setState(State state)
{
    _state = state;
    QItemSelectionModel *selectionModel = ui->list->selectionModel();
    selectionModel->clearSelection();
    if (StateUploadSelection == _state || StateFileActionSelection == _state)
    {
        _selectionCount = 0;
        ui->list->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else
    {
        ui->list->setSelectionMode(QAbstractItemView::NoSelection);
    }
}
