#include <QPainter>
#include <QPixmapCache>

#include "geninterfaces.h"

GenCachedWidget::GenCachedWidget(QWidget *parent)
    :QWidget(parent)
{

}

GenCachedWidget::~GenCachedWidget()
{
    disableCache();
}

void GenCachedWidget::enableCache()
{
    QPixmap pixmap;
    pixmap = QPixmap(size());
    pixmap.fill(Qt::transparent);
    QPainter cachePainter(&pixmap);
    render(&cachePainter);
    QPixmapCache::insert(QString("bla%1").arg((int)this), pixmap);
    QObjectList ch = children();
    QListIterator<QObject*> it(ch);
    QObject *child = 0;
    while(it.hasNext())
    {
        child = it.next();
        if (child->isWidgetType())
            static_cast<QWidget*>(child)->setUpdatesEnabled(false);
    }
}
void GenCachedWidget::disableCache()
{
    QPixmapCache::remove(QString("bla%1").arg((int)this));
    QObjectList ch = children();
    QListIterator<QObject*> it(ch);
    QObject *child = 0;
    while(it.hasNext())
    {
        child = it.next();
        if (child->isWidgetType())
            static_cast<QWidget*>(child)->setUpdatesEnabled(true);
    }
}

void GenCachedWidget::paintEvent(QPaintEvent *e)
{
    QPixmap pixmap;
    if (QPixmapCache::find(QString("bla%1").arg((int)this), pixmap))
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, pixmap);
    }
}

GenTapHandlingInterface::~GenTapHandlingInterface()
{

}
