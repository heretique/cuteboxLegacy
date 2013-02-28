#include <QResizeEvent>
#include <QDesktopServices>
#include <QUrl>

#include "genwelcomeview.h"
#include "ui_genwelcomeview.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genresourcereader.h"
#include "genmenubar.h"
#include "genpublicstrings.h"
#include "genappmanager.h"
#include "genliterals.h"
#include "genlog.h"

GenWelcomeView::GenWelcomeView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL)
{
}

GenWelcomeView::~GenWelcomeView()
{
    delete ui;
}

void GenWelcomeView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    _mainWindow->useImageBackground(true);
    ui = new Ui::GenWelcomeView;
    ui->setupUi(this);
    connect(ui->oldUserButton,
            SIGNAL(clicked()),
            SLOT(oldUserButtonClicked()));
    connect(ui->newUserButton,
            SIGNAL(clicked()),
            SLOT(newUserButtonClicked()));
    connect(ui->dropboxLink,
            SIGNAL(clicked()),
            SLOT(handleDropboxCliked()));
    _mainWindow->statusBar()->showHomeButton(false);
    _mainWindow->statusBar()->showExitButton(false);
    _mainWindow->statusBar()->setText(GenPublicStrings::STR_WELCOME);
}

void GenWelcomeView::deactivate()
{
    delete ui->verticalLayoutWidget;
    ui->verticalLayoutWidget = NULL;
    delete ui;
    ui = NULL;
}

void GenWelcomeView::updateMenuBar(GenMenuBar &menuBar)
{
    menuBar.setMenuActions(ActionNONE, ActionNONE, ActionNONE, ActionEXIT);
    menuBar.setMenuIcons(QPixmap(), QPixmap(), QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton4);
}

bool GenWelcomeView::handleAction(const ActionId actionId)
{
    return false;
}

void GenWelcomeView::oldUserButtonClicked()
{
    activateView("GenWebView", QVariant(), true, GenFromTop);
    GenApplicationManager::instance()->requestAuthTokens();
}

void GenWelcomeView::newUserButtonClicked()
{
    activateView("GenRegisterView", QVariant(), true, GenFromTop);
}

void GenWelcomeView::handleDropboxCliked()
{
    QDesktopServices::openUrl(QUrl(DROPBOX_URL));
}

void GenWelcomeView::resizeEvent(QResizeEvent *e)
{
    if (ui)
        ui->verticalLayoutWidget->resize(e->size());
    QWidget::resizeEvent(e);
}
