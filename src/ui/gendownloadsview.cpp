#include "gendownloadsview.h"
#include "ui_gendownloadsview.h"

GenDownloadsView::GenDownloadsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenDownloadsView)
{
    ui->setupUi(this);
}

GenDownloadsView::~GenDownloadsView()
{
    delete ui;
}
