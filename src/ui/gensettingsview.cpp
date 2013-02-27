#include <QResizeEvent>
#include <QStringListModel>
#include <QCheckBox>

#include "gensettingsview.h"
#include "ui_gensettingsview.h"
#include "genmainview.h"
#include "genresourcereader.h"
#include "genmenubar.h"
#include "genmainwindow.h"
#include "genmenubar.h"
#include "genfilebrowserview.h"
#include "genappmodel.h"
#include "genliterals.h"
#include "genutils.h"
#include "genstatuswidget.h"
#include "gendataengine.h"
#include "gentypes.h"
#include "genpublicstrings.h"
#include "genlog.h"


GenSettingsView::GenSettingsView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL)
{

}

GenSettingsView::~GenSettingsView()
{
    delete ui;
}

void GenSettingsView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenSettingsView;
    ui->setupUi(this);
    if (data.isValid())
        _previousView = data.toString();
    connect(ui->changeLocation,
            SIGNAL(clicked(ActionId)),
            SLOT(handleAction(ActionId)));
    connect(ui->unlinkDevice,
            SIGNAL(clicked(ActionId)),
            SLOT(handleAction(ActionId)));
    ui->changeLocation->setAction(ActionCHANGEDOWNLOADLOCATION);
    ui->unlinkDevice->setAction(ActionUNLINKDEVICE);
    _mainWindow->statusBar()->setText("Settings");
    _mainWindow->statusBar()->showHomeButton(false);
    retrieveSettings();
    connect(ui->autoDownload,
            SIGNAL(stateChanged(int)),
            SLOT(handleCheckStateChanged(int)));
    connect(ui->retrieveThumbnails,
            SIGNAL(stateChanged(int)),
            SLOT(handleCheckStateChanged(int)));
    QTimer::singleShot(1000, this, SIGNAL(getAccountInfo()));
}

void GenSettingsView::deactivate()
{
    delete ui->verticalLayoutWidget;
    ui->verticalLayoutWidget = NULL;
    delete ui;
    ui = NULL;
}

void GenSettingsView::updateMenuBar(GenMenuBar &menuBar)
{
    setSettingsMenuButtons(menuBar);
}

void GenSettingsView::setOKButton()
{
    _mainWindow->menuBar()->setMenuButton(GenMenuBar::GenMenuButton1,
                                          ActionOK,
                                          GenResourceReader::pixmapFromResource("ok_icon.png"));
}

void GenSettingsView::handleModelChanged(GenAppModel &appModel)
{
    GenAccountInfo accountInfo = appModel.getAccountInfo();
    if (this == _mainWindow->currentView() && ui)
        ui->usedSpace->setText(QString("Used space: <b>%1 \%</b>").arg(
                                   getUsedSpacePercent(accountInfo._usedSpace,
                                                       accountInfo._sharedSpace,
                                                       accountInfo._spaceLimit), 0, 'f', 1));
}

bool GenSettingsView::handleAction(const ActionId actionId)
{
    switch(actionId)
    {
    case ActionOK:
    case ActionCANCEL:
        activateView(_previousView, (int)GenMainView::StateSkipUpdate, true, GenFromBottom);
        return true;
    case ActionCHANGEDOWNLOADLOCATION:
        if (!_mainWindow->popupOpened())
        {
            activateView("GenFileBrowserView",
                         GenFileBrowserView::StateFolderSelection,
                         true);
        }
        else
        {
            _mainWindow->showNotificationInfo(GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS);
        }
        return true;
    case ActionUNLINKDEVICE:
        if (!_mainWindow->popupOpened())
        {
            _appModel->setSettingValue(SETTINGS_EMAIL, "");
            _appModel->setSettingValue(SETTINGS_OAUTH_TOKEN, "");
            _appModel->setSettingValue(SETTINGS_OAUTH_TOKEN_SECRET, "");
            _appModel->dbEngine().purgeAndClose();
            _appModel->dbEngine().init();
            activateView("GenWelcomeView", QVariant(), true, GenFromLeft);
        }
        else
        {
            _mainWindow->showNotificationInfo(GenPublicStrings::STR_ANOTHER_ACTION_IN_PROGRESS);
        }
        return true;
    case ActionHELP:
        activateView("GenHelpView", QVariant(), true, GenFromBottom);
        return true;
    default:
        break;
    }

    return false;
}

void GenSettingsView::handleCheckStateChanged(int state)
{
    bool checked = false;
    checked = Qt::Checked == ui->retrieveThumbnails->checkState();
    _appModel->setSettingValue(SETTINGS_AUTOGETTHUMBS, checked);
    checked = Qt::Checked == ui->autoDownload->checkState();
    _appModel->setSettingValue(SETTINGS_AUTODOWNLOAD, checked);
}

void GenSettingsView::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if (ui)
        ui->verticalLayoutWidget->resize(e->size());
}

void GenSettingsView::retrieveSettings()
{
    GenAccountInfo accountInfo = _appModel->getAccountInfo();
    ui->usedSpace->setText(QString("Used space: <b>%1 \%</b>").arg(
                               getUsedSpacePercent(accountInfo._usedSpace,
                                                   accountInfo._sharedSpace,
                                                   accountInfo._spaceLimit), 0, 'f', 1));
    ui->email->setText(QString("Email: <b>%1</b>").arg(_appModel->settingValue(SETTINGS_EMAIL).toString()));
    ui->donwloadPath->setText(
                QString("Download to: <b>%1</b>").arg(_appModel->settingValue(SETTINGS_DOWNLOAD_LOCATION).toString()));
    ui->retrieveThumbnails->setChecked(_appModel->autoGetTumbs());
    ui->autoDownload->setChecked(_appModel->autoDownload());
}

qreal GenSettingsView::getUsedSpacePercent(qint64 usedSpace, qint64 sharedSpace, qint64 totalSpace)
{
    return ((qreal)usedSpace * 100.0 / totalSpace) + ((qreal)sharedSpace * 100.0 / totalSpace);
}
