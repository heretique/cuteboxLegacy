#include <QResizeEvent>
#include <QScrollBar>

#include "genphotoview.h"
#include "ui_genphotoview.h"

GenPhotoView::GenPhotoView(GenWSEngine *wsEngine,
                           GenAppModel *appModel,
                           GenMainWindow* mainWindow,
                           QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent),
    ui(NULL)
{

}

GenPhotoView::~GenPhotoView()
{
    delete ui;
}


void GenPhotoView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenPhotoView;
    ui->setupUi(this);
    ui->photoBrowser->initialize();
}

void GenPhotoView::deactivate()
{
    delete ui->photoBrowser;
    ui->photoBrowser = NULL;
    delete ui;
    ui = NULL;
}

void GenPhotoView::updateMenuBar(GenMenuBar &menuBar)
{

}

bool GenPhotoView::handleAction(const ActionId actionId)
{
    return false;
}

void GenPhotoView::resizeEvent(QResizeEvent *e)
{
    if (ui)
        ui->photoBrowser->resize(e->size());
    QWidget::resizeEvent(e);
}

