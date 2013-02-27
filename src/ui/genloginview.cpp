#include <QResizeEvent>

#include "genloginview.h"
#include "ui_genloginview.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genmenubar.h"
#include "genresourcereader.h"
#include "genutils.h"
#include "genlog.h"


GenLoginView::GenLoginView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL)
{

}

GenLoginView::~GenLoginView()
{
    delete ui;
}

void GenLoginView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenLoginView;
    ui->setupUi(this);
    ui->userEditCtrl->setText("");
    ui->passEditCtrl->setText("");
    _mainWindow->statusBar()->setText("Enter your credentials");
    connect(ui->passEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
    connect(ui->userEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
}

void GenLoginView::loginClicked()
{
    QLOG_DEBUG("GenWelcomeView::on_login_clicked()");
    QString userName = ui->userEditCtrl->text();
    QString userPass = ui->passEditCtrl->text();
    emit startOauthTokenRequest(userName, userPass);
}

void GenLoginView::handleEditTextChanged(QString text)
{
    if (!ui->userEditCtrl->text().isEmpty() && !ui->passEditCtrl->text().isEmpty())
    {
        if (!_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1))
            setOKCancelMenuButtons(*(_mainWindow->menuBar()));
    }
    else
    {
        if (_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1))
            setCancelMenuButton(*(_mainWindow->menuBar()));
    }
}

void GenLoginView::deactivate()
{
    delete ui->verticalLayoutWidget;
    ui->verticalLayoutWidget = NULL;
    delete ui;
    ui = NULL;
}

void GenLoginView::updateMenuBar(GenMenuBar &menuBar)
{
    setCancelMenuButton(menuBar);
}

bool GenLoginView::handleAction(const ActionId actionId)
{
    if (ActionCANCEL == actionId)
    {
        activateView("GenWelcomeView", QVariant(), true, GenFromBottom);
        return true;
    }
    if (ActionOK == actionId)
    {
        loginClicked();
    }
    return false;
}

void GenLoginView::resizeEvent(QResizeEvent *e)
{
    if (ui)
        ui->verticalLayoutWidget->resize(e->size());
    QWidget::resizeEvent(e);
}
