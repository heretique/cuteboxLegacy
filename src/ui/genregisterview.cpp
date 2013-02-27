#include <QResizeEvent>

#include "genregisterview.h"
#include "ui_genregisterview.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genmenubar.h"
#include "genutils.h"

GenRegisterView::GenRegisterView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL)
{

}

GenRegisterView::~GenRegisterView()
{
    delete ui;
}

void GenRegisterView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenRegisterView;
    ui->setupUi(this);
    _mainWindow->statusBar()->setText("Enter registration information");
    connect(ui->passEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
    connect(ui->emailEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
    connect(ui->firstNameEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
    connect(ui->lastNameEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
    connect(ui->confirmPassEditCtrl,
            SIGNAL(textEdited(QString)),
            SLOT(handleEditTextChanged(QString)));
}

void GenRegisterView::deactivate()
{
    delete ui->verticalLayoutWidget;
    ui->verticalLayoutWidget = NULL;
    delete ui;
    ui = NULL;
}

void GenRegisterView::updateMenuBar(GenMenuBar &menuBar)
{
    setCancelMenuButton(menuBar);
}

bool GenRegisterView::handleAction(const ActionId actionId)
{
    if (ActionCANCEL == actionId)
    {
        activateView("GenWelcomeView", QVariant(), true, GenFromBottom);
        return true;
    }
    if (ActionOK == actionId)
    {
        emit registerAccount(ui->firstNameEditCtrl->text(),
                             ui->lastNameEditCtrl->text(),
                             ui->emailEditCtrl->text(),
                             ui->passEditCtrl->text());
    }
    return false;
}

void GenRegisterView::resizeEvent(QResizeEvent *e)
{
    if (ui)
        ui->verticalLayoutWidget->resize(e->size());
    QWidget::resizeEvent(e);
}

void GenRegisterView::handleEditTextChanged(QString text)
{
    if (!ui->emailEditCtrl->text().isEmpty() &&
            !ui->passEditCtrl->text().isEmpty() &&
            !ui->firstNameEditCtrl->text().isEmpty() &&
            !ui->lastNameEditCtrl->text().isEmpty() &&
            !ui->confirmPassEditCtrl->text().isEmpty())
    {
        if (!_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1) &&
                (ui->passEditCtrl->text() == ui->confirmPassEditCtrl->text()) &&
                ui->passEditCtrl->text().size() >= 6)
        {
            setOKCancelMenuButtons(*(_mainWindow->menuBar()));
            _mainWindow->statusBar()->setText("Enter registration information");
        }
        else if (ui->passEditCtrl->text().size() < 6)
        {
            if (_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1))
                setCancelMenuButton(*(_mainWindow->menuBar()));
            _mainWindow->statusBar()->setText("Need >6 characters password");
        }
        else if (ui->passEditCtrl->text() != ui->confirmPassEditCtrl->text())
        {
            if (_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1))
                setCancelMenuButton(*(_mainWindow->menuBar()));
            _mainWindow->statusBar()->setText("Passwords don't match");
        }
    }
    else
    {
        if (_mainWindow->menuBar()->buttons().testFlag(GenMenuBar::GenMenuButton1))
            setCancelMenuButton(*(_mainWindow->menuBar()));
    }
}

