#include <QApplication>
#include <QDesktopWidget>
#include <QTouchEvent>
#include <QGraphicsRectItem>
//#include <QtOpenGL/QGLWidget>

#include "genphotobrowser.h"
#include "genpannablewall.h"
#include "genlog.h"

GenPhotoBrowser::GenPhotoBrowser(QWidget *parent)
    : QGraphicsView(parent)
{
    //    setupViewport(new QGLWidget());
    setAttribute(Qt::WA_TranslucentBackground, false);
    QGraphicsScene *scene = new QGraphicsScene;
    setScene(scene);
    setSceneRect(rect());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(QColor(238, 238, 238));
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setFrameStyle(0);
//    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
}

GenPhotoBrowser::~GenPhotoBrowser()
{

}

class GenTestGraphicsWidget : public QGraphicsWidget
{
public:
    QColor _color;
    GenTestGraphicsWidget(QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent)
    {
        _color = QColor(QColor::colorNames()[qrand() % QColor::colorNames().count()]);
        //        setCacheMode(QGraphicsItem::ItemCoordinateCache);
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)
        painter->setBrush(_color);
        painter->drawRect(rect());
    }
};


void GenPhotoBrowser::initialize()
{
    GenPannableWall *wall = new GenPannableWall(Qt::Vertical,
                                                rect().width(),
                                                rect().height(),
                                                false);
    scene()->addItem(wall);
//    for (int i = 0;  i < 500; ++i)
//    {
//        GenTestGraphicsWidget *item = new GenTestGraphicsWidget();
//        item->resize(128, 128);
//        item->setPreferredSize(128, 128);
//        wall->addWidget(item);
//    }
    GenTestGraphicsWidget *item = new GenTestGraphicsWidget();
    item->resize(200, 200);
    scene()->addItem(item);
    wall->setScrollableWidget(item);
}

void GenPhotoBrowser::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    QLOG_DEBUG("GenPhotoBrowser::resizeEvent - rect: " << rect());
    setSceneRect(rect());
}
