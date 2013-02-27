#include "genfilenamepopup.h"
#include "ui_genfilenamepopup.h"
#include "genconstants.h"
#include "genmenubar.h"
#include "genappmodel.h"
#include "genresourcereader.h"
#include "genutils.h"

GenFilenamePopup::GenFilenamePopup(GenMenuBar &menuBar, GenAppModel &appModel, QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenFilenamePopup),
    _menuBar(menuBar),
    _appModel(appModel),
    _nameEmpty(true)
{
    ui->setupUi(this);
}

GenFilenamePopup::~GenFilenamePopup()
{
    delete ui;
}

void GenFilenamePopup::updateMenuBar(GenMenuBar &menuBar)
{
    _nameEmpty = true;
    setCancelMenuButton(menuBar);
}

bool GenFilenamePopup::handleAction(const ActionId actionId)
{
    if (ActionOK == actionId)
    {
        _appModel.setNewFolderName(ui->lineEdit->text());
        ui->lineEdit->clear();
        emit popupClosed(ActionCREATEFOLDER);
        return true;
    }
    else if (ActionCANCEL == actionId)
    {
        ui->lineEdit->clear();
        emit popupClosed(ActionCANCEL);
        return true;
    }

    return false;
}

void GenFilenamePopup::on_lineEdit_textChanged(QString text)
{
    if (text.isEmpty())
    {
        if (!_nameEmpty)
            updateMenuBar(_menuBar);
        _nameEmpty = true;
    }
    else
    {
        if (_nameEmpty)
        {
            _nameEmpty = false;

            setOKCancelMenuButtons(_menuBar);
        }
    }
}
