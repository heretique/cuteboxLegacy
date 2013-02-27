#include "genwaitpopup.h"
#include "ui_genwaitpopup.h"

GenWaitPopup::GenWaitPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenWaitPopup)
{
    ui->setupUi(this);
}

GenWaitPopup::~GenWaitPopup()
{
    delete ui;
}

void GenWaitPopup::start()
{
    ui->waitIcon->start();
}

void GenWaitPopup::stop()
{
    ui->waitIcon->stop();
}

void GenWaitPopup::setText(QString text)
{
    ui->text->setText(text);
}
