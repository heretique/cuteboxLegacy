#include <QGraphicsGridLayout>
#include <QGraphicsScene>
#include <QPainter>
#include <QList>
#include <QListIterator>

#include "genpannablewidget.h"
#include "gencommondefs.h"
#include "geninterfaces.h"
#include "genlog.h"

GenPannableWidget::GenPannableWidget(Qt::Orientation orientation, QGraphicsObject * parent) :
    QGraphicsWidget(parent)
{
    setObjectName("GenPannableWidget");

    setFlag(QGraphicsItem::ItemStacksBehindParent);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    layout->setSpacing(30);
    layout->setContentsMargins(30, 0, 30, 0);
    setLayout(layout);
}

GenPannableWidget::~GenPannableWidget()
{
    removeAllItems(true);
}

void GenPannableWidget::paint(QPainter *painter,
        const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

#ifdef DRAW_BOUNDARIES
    painter->setPen(Qt::green);
    painter->drawRect(rect());
#endif // DRAW_BOUNDARIES
}

QPainterPath GenPannableWidget::shape() const
{
    QPainterPath path;
    QSizeF size = QGraphicsWidget::size();
    path.addRect(QRectF(0, 0, size.width(), size.height()));
    return path;
}

void GenPannableWidget::addWidget(QGraphicsWidget *widget, int pos)
{
    if (!widget)
    {
        return;
    }
    QGraphicsGridLayout* l = static_cast<QGraphicsGridLayout*> (layout());
    l->addItem(widget, l->count() / 2, l->count() % 2);
}

void GenPannableWidget::addItem(QGraphicsLayoutItem *item, int pos)
{
    if (!item)
    {
        return;
    }

    QGraphicsGridLayout* l = static_cast<QGraphicsGridLayout*> (layout());
    l->addItem(item, l->count() / 2, l->count() % 2);
}

void GenPannableWidget::removeAt(int pos)
{
    layout()->removeAt(pos);
}

QGraphicsWidget* GenPannableWidget::itemAt(int pos)
{
    return static_cast<QGraphicsWidget*> (layout()->itemAt(pos));
}

void GenPannableWidget::removeAllItems(bool destroy)
{
    int count = layout()->count();
    for (int i = count - 1; i >= 0; --i)
    {
        QGraphicsLayoutItem* widget = layout()->itemAt(i);
        layout()->removeAt(i);
        if (destroy)
        {
            delete widget;
        }
    }
}

int GenPannableWidget::count() const
{
    return layout()->count();
}

void GenPannableWidget::setContentsMargins(int left, int top, int right,
        int bottom)
{
    QLOG_DEBUG("GenPannableWidget::setContentsMargins() - " << left <<" "
               << top << " " << right << " " << bottom);
    layout()->setContentsMargins(left, top, right, bottom);
}

void GenPannableWidget::longTap(QPointF &pos)
{
    QLOG_DEBUG("PannableWidget::longTap - pos = " << pos);
    QList<QGraphicsItem*> items = childItems();
    QListIterator<QGraphicsItem*> it(items);

    while (it.hasNext())
    {
        QGraphicsItem* item = it.next();
        QLOG_DEBUG("Item " << item << "boundingRect = " << item->boundingRect());
        if (item->contains(item->mapFromScene(pos)))
        {
            GenTapHandlingInterface* tapHandlingItem =
                    dynamic_cast<GenTapHandlingInterface*> (item);
            if (0 != tapHandlingItem)
            {
                tapHandlingItem->handleLongTap(pos);
                break;
            }
        }
    }
}

void GenPannableWidget::tap(const QPointF &pos)
{
    QLOG_DEBUG("PannableWidget::tap - pos = " << pos);
    QList<QGraphicsItem*> items = childItems();
    QListIterator<QGraphicsItem*> it(items);
    while (it.hasNext())
    {
        QGraphicsItem* item = it.next();
        QLOG_DEBUG("Item " << item << "boundingRect = " << item->boundingRect());
        if (item->contains(item->mapFromScene(pos)))
        {
            GenTapHandlingInterface* tapHandlingItem =
                    dynamic_cast<GenTapHandlingInterface*> (item);
            if (0 != tapHandlingItem)
            {
                tapHandlingItem->handleTap(pos);
                break;
            }
        }
    }
}
