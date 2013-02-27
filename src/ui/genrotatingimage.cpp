#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include "genrotatingimage.h"

GenRotatingImage::GenRotatingImage(QWidget *parent)
    : QWidget(parent), _timer(new QTimer(this))
{
    connect(_timer, SIGNAL(timeout()), this, SLOT(rotateImage()));
}

GenRotatingImage::~GenRotatingImage()
{

}


void GenRotatingImage::paintEvent(QPaintEvent *e)
{
    static qreal angle = 0.0;
    ++angle;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(QPoint(_pixmap.width() / 2, _pixmap.height() / 2));
    painter.rotate(angle);
    painter.drawPixmap( - _pixmap.width() / 2, - _pixmap.height() / 2, _pixmap);
}

void GenRotatingImage::setPixmap(QPixmap pixmap)
{
    _pixmap = pixmap;
}

void GenRotatingImage::setText(QString t)
{

}

void GenRotatingImage::rotateImage()
{
    update();
}

void GenRotatingImage::start()
{
    _timer->start(0);
}

void GenRotatingImage::stop()
{
    _timer->stop();
}
