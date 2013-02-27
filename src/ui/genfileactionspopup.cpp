#include "genfileactionspopup.h"
#include "ui_genfileactionspopup.h"

GenFileActionsPopup::GenFileActionsPopup(QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenFileActionsPopup)
{
    ui->setupUi(this);
//    ui->copyButton->setAction(ActionCOPYFILES);
//    ui->moveButton->setAction(ActionMOVEFILES);
//    ui->deleteButton->setAction(ActionDELETEFILES);
//    ui->uploadDownloadButton->setAction(ActionDOWNLOADFILES);
    connect(ui->copyButton, SIGNAL(clicked(ActionId)), SIGNAL(actionTriggered(ActionId)));
    connect(ui->moveButton, SIGNAL(clicked(ActionId)), SIGNAL(actionTriggered(ActionId)));
    connect(ui->deleteButton, SIGNAL(clicked(ActionId)), SIGNAL(actionTriggered(ActionId)));
    connect(ui->uploadDownloadButton, SIGNAL(clicked(ActionId)), SIGNAL(actionTriggered(ActionId)));
}

GenFileActionsPopup::~GenFileActionsPopup()
{
    delete ui;
}

const GenPushButton *GenFileActionsPopup::copyButton()
{
    return ui->copyButton;
}

const GenPushButton *GenFileActionsPopup::moveButton()
{
    return ui->moveButton;
}

const GenPushButton *GenFileActionsPopup::deleteButton()
{
    return ui->deleteButton;
}

const GenPushButton *GenFileActionsPopup::uploadDownloadButton()
{
    return ui->uploadDownloadButton;
}
