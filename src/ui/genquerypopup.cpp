#include "genquerypopup.h"
#include "ui_genquerypopup.h"
#include "genconstants.h"

GenQueryPopup::GenQueryPopup(QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenQueryPopup)
{
    ui->setupUi(this);
}

GenQueryPopup::~GenQueryPopup()
{
    delete ui;
}

void GenQueryPopup::setText(const QString &text)
{
    ui->text->setText(text);
}

void GenQueryPopup::setAction(const ActionId actionId)
{
    _actionId = actionId;
}

const ActionId GenQueryPopup::action() const
{
    return _actionId;
}

bool GenQueryPopup::handleAction(const ActionId actionId)
{
    if (ActionOK == actionId)
    {
        emit popupClosed(_actionId);
        return true;
    }
    else if (ActionCANCEL == actionId)
    {
        emit popupClosed(ActionCANCEL);
        return true;
    }

    return false;
}


