#ifndef GENPANNABLEWALL_H
#define GENPANNABLEWALL_H

#include <QGraphicsWidget>
#include <QTimeLine>
#include <QTime>

class QTimeLine;
class QGraphicsRotation;
class QPropertyAnimation;

class GenPannableWall : public QGraphicsWidget
{
    Q_OBJECT

public:

    enum SwipeDirection
    {
        Unknown = 0,
        Left,
        Right,
        Top,
        Bottom
    };

    GenPannableWall(Qt::Orientation orientation,
                    qreal width,
                    qreal height,
                    bool perspective,
                    QGraphicsWidget * parent = 0);
    GenPannableWall(Qt::Orientation orientation,
                    bool perspective,
                    QGraphicsWidget * parent);
    ~GenPannableWall();
    void setScrollableWidget(QGraphicsWidget *scrollableItem);
    void removeScrollableWidget();
    bool isScrolling() const;
    void moveToBegin();
    void moveToEnd();
    void enableScrollIndicator(bool enable);
    Qt::Orientations orientation() const;
    void ensureVisible(const QRectF &rect);

public slots:
    void setOrientation(Qt::Orientation orientation);

protected:
    void startScrolling(int duration = 1000000);
    void stopScrolling(int clear = true);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void tap(QGraphicsSceneMouseEvent * event);
    void swipe(QPointF const & startPoint, QPointF const &endPoint);
    void animateEnd(QList<QPointF> const & points, int duration = 800);
    void storeSwipeStyle(QPointF const &first, QPointF const &second);
    bool endReached();
    void moveTo(QPointF const & pos);
    void recalculateGeometry();
    void initialize();
    void rotateWall();
    void easeWallAngle();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);


protected slots:
    void scroll(qreal value);
    void emitPositionChanged();
    void finishedEaseAngle();
    void scrollingTimelineStoped();

signals:
    void posChanged(QRectF const & viewRect);
    void tapped(QPointF const &point);
    void stopAnimation();
    void flicked(GenPannableWall::SwipeDirection direction);

private:
    QGraphicsWidget *_scrollableWidget;
    qreal _deltaX;
    qreal _deltaY;
    qreal _v;
    int _minX;
    int _minY;
    int _maxX;
    int _maxY;
    QTime _gestureTime;
    QTimeLine *_scrollingTimeLine;
    Qt::Orientation _orientation;
    bool _skipTapGesture;
    bool _enableIndicator;
    QGraphicsRotation* _scrollableWidgetRotation;
    qreal _wallAngle;
    bool _usePerspective;
    bool _isEasingAngle;
    QPropertyAnimation *_easeAnim;
};


#endif // GENPANNABLEWALL_H
