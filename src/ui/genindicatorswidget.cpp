#include <QTime>
#include <QResizeEvent>

#include "genindicatorswidget.h"
#include "ui_genindicatorswidget.h"

GenIndicatorsWidget::GenIndicatorsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenIndicatorsWidget)
{
    ui->setupUi(this);
    ui->batteryLevel->setMaximum(100);
    ui->signalLevel->setMaximum(100);
}

GenIndicatorsWidget::~GenIndicatorsWidget()
{
    delete ui;
}


void GenIndicatorsWidget::setBatteryLevel(int level)
{
    ui->batteryLevel->setValue(level);
}

void GenIndicatorsWidget::setSignalLevel(int level)
{
    ui->signalLevel->setValue(level);
}

void GenIndicatorsWidget::setClock(QTime clock)
{
    ui->clock->setText(clock.toString("HH:mm"));
}

void GenIndicatorsWidget::resizeEvent(QResizeEvent *e)
{
    if (ui)
    {
        ui->horizontalLayoutWidget->resize(e->size());
    }
    QWidget::resizeEvent(e);
}
