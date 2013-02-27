#include "genfileprogress.h"
#include "ui_genfileprogress.h"

GenFileProgress::GenFileProgress(QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenFileProgress)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), SIGNAL(fileActionCanceled()));
}

GenFileProgress::~GenFileProgress()
{
    delete ui;
}

void GenFileProgress::setText(const QString &text)
{
    QLabel *label = ui->text;
    QFontMetrics fontMetrics(label->font());
    ui->text->setText(fontMetrics.elidedText(text, Qt::ElideRight, label->width()));
}

void GenFileProgress::setItemProgress(qint64 bytes, qint64 bytesTotal)
{
    ui->itemProgressBar->setMaximum(bytesTotal);
    ui->itemProgressBar->setValue(bytes);
    // calculate the download speed
    double speed = bytes * 1000.0 / _fileProgressTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }
    ui->speed->setText(QString::fromLatin1("Speed: %1 %2").arg(speed, 3, 'f', 1).arg(unit));
}

void GenFileProgress::setTotalProgress(int current, int total)
{
    ui->totalProgressBar->setMaximum(total);
    ui->totalProgressBar->setValue(current);
}

void GenFileProgress::showTotalProgress(bool show)
{
    show ? ui->totalProgressBar->show() : ui->totalProgressBar->hide();
}

void GenFileProgress::start()
{
    _fileProgressTime.start();
}

void GenFileProgress::reset()
{
    ui->itemProgressBar->reset();
    ui->totalProgressBar->reset();
}
