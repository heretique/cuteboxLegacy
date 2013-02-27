#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QPointF>
#include <QRectF>
#include <QTimeLine>
#include <math.h>
#include <QPaintEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLayout>
#include <QPropertyAnimation>
#include <QList>
#include <QPainter>
#include <QGraphicsRotation>

// Only for testing.
#include <QCursor>

#include "GenPannableWall.h"
#include "genpannablewidget.h"
#include "genresourcereader.h"
#include "genconstants.h"
#include "genlog.h"

#define PANNABLE_FRICTION_COEFFICIENT 0.15 * 9.81  // F = y*N, 0.1 = friction coefficient for ice on ice.
#define TAP_GESTURE_THRESHOLD 10
#define VELOCITY_THRESHOLD 0.5
#define HIGH_VEL_THRESHOLD 100
#define MAX_ANGLE 45
#define MIN_ANGLE -45
#define ANGLE_COEF 0.01
#define MSECS 1000
#define SCROLL_MIN_THRESHOLD 0
#define SCROLL_MAX_THRESHOLD 0.35
#define SLOW_PERCENT -0.4
#define SWIPE_GESTURE_THRESHOLD 150



GenPannableWall::GenPannableWall(Qt::Orientation orientation,
                                 qreal width,
                                 qreal height,
                                 bool usePerspective,
                                 QGraphicsWidget *parent) :
    QGraphicsWidget(parent),
    _scrollableWidget(NULL),
    _orientation(orientation),
    _usePerspective(usePerspective),
    _isEasingAngle(false),
    _easeAnim(NULL)
{
    resize(width, height);
}

GenPannableWall::GenPannableWall(Qt::Orientation orientation,
                                 bool usePerspective,
                                 QGraphicsWidget *parent) :
    QGraphicsWidget(parent),
    _scrollableWidget(NULL),
    _orientation(orientation),
    _usePerspective(usePerspective),
    _isEasingAngle(false),
    _easeAnim(NULL)
{

}

GenPannableWall::~GenPannableWall()
{

}

void GenPannableWall::setScrollableWidget(QGraphicsWidget *scrollableWidget)
{
    _scrollableWidget = scrollableWidget;
    _scrollableWidget->setParentItem(this);
    _scrollableWidget->setFlag(QGraphicsItem::ItemStacksBehindParent);
    initialize();
}

void GenPannableWall::removeScrollableWidget()
{
    delete _scrollableWidget;
    _scrollableWidget = 0;
}

void GenPannableWall::startScrolling(int duration)
{
    _scrollingTimeLine->setCurrentTime(0);
    _scrollingTimeLine->setDuration(duration);
    _scrollingTimeLine->start();
}

void GenPannableWall::stopScrolling(int clear)
{
    QLOG_DEBUG("STOP SCROLLING");
    _scrollingTimeLine->stop();

    if (clear)
    {
        _scrollingTimeLine->setCurrentTime(0);
        _v = 0;
    }
    if (_usePerspective)
        easeWallAngle();

    emitPositionChanged();
}

void GenPannableWall::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(_scrollableWidget);
    QLOG_DEBUG("GenPannableWall::mouseMoveEvent");

    emit stopAnimation();

    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    if (_scrollingTimeLine->state() == QTimeLine::Running)
    {
        stopScrolling(false);
        _skipTapGesture = true; // We don't want to stop and tap. We want stop or tap
    }

    _gestureTime.start();
}

void GenPannableWall::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(_scrollableWidget);
    QLOG_DEBUG("GenPannableWall::mouseMoveEvent");
    _deltaX = 0;
    _deltaY = 0;
    if (_isEasingAngle)
    {
        _easeAnim->stop();
        _isEasingAngle = false;
    }
    qint32 moveTime = _gestureTime.elapsed();
    QPointF moveDelta = event->pos() - event->buttonDownPos(Qt::LeftButton);
    qreal dx = moveDelta.x();
    qreal dy = moveDelta.y();

    QLOG_DEBUG("mouseMoveEvent - _moveTime = " << moveTime);

    QPointF delta = event->pos() - event->lastPos();
    if (_orientation & Qt::Horizontal)
    {
        _deltaX = delta.x();
        _v = (dx  / moveTime) * MSECS;
    }

    if (_orientation & Qt::Vertical)
    {
        _deltaY = delta.y();
        _v = (dy  / moveTime) * MSECS;
    }

    // translate the view by delta
    _scrollableWidget->moveBy(_deltaX, _deltaY);
    // rotate
    if (_usePerspective)
        rotateWall();
}

void GenPannableWall::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(_scrollableWidget);
    QLOG_DEBUG("GenPannableWall::mouseReleaseEvent");
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    // For some weird reason the mouseGrapper thingy makes the buttonDownPos() to return
    // exactly the same as scenePos() therefore we need to store mouse press position and
    // use it here.
    QPointF pressPos = event->buttonDownPos(Qt::LeftButton);
    QPointF releasePos = event->pos();

    QPointF delta = pressPos - releasePos;
    qreal distance = sqrt(pow(delta.x(), 2) + pow(delta.y(), 2));

    if (distance < TAP_GESTURE_THRESHOLD && !_skipTapGesture)
    {
        // This is a tap gesture
        tap(event);
    }
    else
    {
        // This is a swipe gesture
        swipe(pressPos, releasePos);
    }
}

void GenPannableWall::tap(QGraphicsSceneMouseEvent * event)
{
    endReached();

    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    QGraphicsSceneMouseEvent press(QEvent::GraphicsSceneMousePress);
    QGraphicsSceneMouseEvent release(QEvent::GraphicsSceneMouseRelease);

    press.setScenePos(event->scenePos());
    press.setScreenPos(event->screenPos());
    press.setButton(event->button());
    press.setPos(event->pos());

    release.setScenePos(event->scenePos());
    release.setScreenPos(event->screenPos());
    release.setButton(event->button());
    release.setPos(event->pos());

    // Send event to the scrollable item
    scene()->sendEvent(_scrollableWidget, &press);
    scene()->sendEvent(_scrollableWidget, &release);

    emit tapped(event->pos());
}

void GenPannableWall::swipe(QPointF const & startPoint, QPointF const &endPoint)
{
    _skipTapGesture = false;
    QPointF delta = endPoint - startPoint;
    qint32 tD = _gestureTime.elapsed(); // Delta Time
    qreal dxD = delta.x(); // Delta X distance
    qreal dyD = delta.y(); // Delta Y distance


    qreal vx = (dxD / tD) * MSECS; // Velocity in x dimension (pixels / second).
    qreal vy = (dyD / tD) * MSECS; // Velocity in y dimension (pixels / second).

    // Store swipe style for later use
    storeSwipeStyle(startPoint, endPoint);

    if (fabs(vx) > VELOCITY_THRESHOLD || fabs(vy) > VELOCITY_THRESHOLD)
    {
        qreal velocity = 0; // Current velocity in x or y dimension.

        _deltaX = dxD;
        _deltaY = dyD;

        if (_orientation & Qt::Horizontal)
        {
            velocity = fabs(vx);
            _v = vx;
        }
        else
        {
            velocity = fabs(vy);
            _v = vy;
        }

        if (velocity > 0 && !endReached())
        {
            // Actually this duration doesn't effect on anything at the moment.
            startScrolling(static_cast<int> (velocity
                                             / PANNABLE_FRICTION_COEFFICIENT));
        }
    }
}

void GenPannableWall::scroll(qreal value)
{
    Q_UNUSED(value);

    // Check first if we have reached one of the ends


    if (endReached())
    {
        return;
    }

    qreal sX = 0, sY = 0;
    // s = 1/2 * (u + v)t,
    // u=0,
    // t=update interval (in ms) of the QTimeLine.
    qreal s = _v * (_scrollingTimeLine->updateInterval() / 2) / MSECS;

    // v = u + at,
    // u = d_ptr->m_vX (current speed),
    // a = yN/m, m = 1kg (just to have something), yN = friction
    qreal v = (fabs(_v) - (PANNABLE_FRICTION_COEFFICIENT
                           * _scrollingTimeLine->updateInterval()));
    _v = (_v < 0 ? -v : v);

    if (fabs(s) > SCROLL_MIN_THRESHOLD && fabs(s) < SCROLL_MAX_THRESHOLD)
    {
        stopScrolling();
    }

    if (_orientation == Qt::Horizontal)
    {
        sX = s;
    }
    else if (_orientation == Qt::Vertical)
    {
        sY = s;
    }

    //    QLOG_DEBUG("scroll - velocity =" << _v);
    _scrollableWidget->moveBy(sX * 2, sY * 2);
    if (_usePerspective)
        rotateWall();

    emitPositionChanged();
}

void GenPannableWall::animateEnd(QList<QPointF> const & points, int duration)
{

    if (points.isEmpty())
    {
        return;
    }

    QPropertyAnimation *animation = new QPropertyAnimation(_scrollableWidget,
                                                           "pos");
    qreal step = 1.0 / (points.size() - 1);
    qreal value = 0;

    Q_FOREACH( QPointF point, points)
    {
        animation->setKeyValueAt( value, point);
        value += step;
    }

    animation->setEasingCurve(QEasingCurve::OutQuart);
    animation->setDuration(duration);
    connect(animation, SIGNAL(finished()), this, SLOT(emitPositionChanged()));
    connect(this, SIGNAL(stopAnimation()), animation, SLOT(stop()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GenPannableWall::storeSwipeStyle(QPointF const &first, QPointF const &second)
{
}

bool GenPannableWall::endReached()
{
    //    QLOG_DEBUG("GenPannableWall::endReached()");
    QPointF pos = _scrollableWidget->pos();
    qreal widgetX = pos.x();
    qreal widgetY = pos.y();
    qreal margin = 0;

    QRectF limitRect(QPointF(widgetX, widgetY), _scrollableWidget->size());
    QRectF viewRect = boundingRect();
    int viewHeight = viewRect.height();
    int viewWidth = viewRect.width();

    // Flags to indicate if e.g. the limit rect height or width is less than viewRect's
    // Then we can't use that value as a comparison because it will be true always.
    bool checkWidth = limitRect.width() >= viewWidth;
    bool checkHeight = limitRect.height() > viewHeight;

    if (!limitRect.contains(viewRect))
    {

        QRectF intersection = limitRect.intersected(viewRect);
        QList<QPointF> points;
        points << pos;
        int width = intersection.width();
        int height = intersection.height();

        // Handle the situation where we have intersection in corners
        if (width >= margin && width < viewWidth && height >= margin && height
                < viewHeight)
        {
            stopScrolling();
            int x = widgetX, y = widgetY;
            if (checkWidth && _deltaX < 0)
            {
                x = -_maxX;
            }
            else
            {
                x = _minX;
            }

            if (checkHeight && _deltaY < 0)
            {
                y = -_maxY;
            }
            else
            {
                y = _minY;
            }
            points << QPointF(x, y);
            animateEnd(points);
            return true;
        }

        // Handle the situation where only left, top, right or bottom
        // has been intersected.
        if (checkWidth && width >= margin && width < viewWidth)
        {
            stopScrolling();
            if (_deltaX < 0)
            {
                //Panning to the right ->
                points << QPointF(-_maxX, widgetY);
            }
            else
            {
                //Panning to the left <-
                points << QPointF(_minX, widgetY);
            }
            animateEnd(points);
            return true;
        }

        if (checkHeight && height > margin && height < viewHeight)
        {
            stopScrolling();
            if (_deltaY < 0 && checkHeight)
            {
                //Panning to the down
                points << QPointF(widgetX, -_maxY);
            }
            else
            {
                //Panning to the top
                points << QPointF(widgetX, _minY);
            }
            animateEnd(points);
            return true;
        }

    }
    return false;
}

void GenPannableWall::emitPositionChanged()
{
    QPointF point = _scrollableWidget->pos() * -1.0;
    emit posChanged(QRectF(point, boundingRect().size()));

    // TODO: Add here indicator updating
}

void GenPannableWall::moveToBegin()
{
    qreal x = _scrollableWidget->pos().x();
    qreal y = _scrollableWidget->pos().y();
    if (Qt::Horizontal == _orientation)
        x= 0;
    else if (Qt::Vertical == _orientation)
        y = 0;
    _scrollableWidget->setPos(x, y);
}

void GenPannableWall::moveToEnd()
{
    QPointF pos;
    pos.setX(_scrollableWidget->pos().x());
    pos.setY(-(_scrollableWidget->size().height()
               - size().height()));
    _scrollableWidget->setPos(pos);
}

bool GenPannableWall::isScrolling() const
{
    return (_scrollingTimeLine->state() == QTimeLine::Running);
}

void GenPannableWall::moveTo(QPointF const & pos)
{
    QPointF newPos;
    if (_orientation == Qt::Vertical)
    {
        newPos.setY(-pos.y());
    }
    if (_orientation == Qt::Horizontal)
    {
        newPos.setX(-pos.x());
    }

    if (isScrolling())
    {
        stopScrolling();
    }

    QList<QPointF> points;
    points << _scrollableWidget->pos() << newPos;
    animateEnd(points);
}

void GenPannableWall::recalculateGeometry()
{
    QRectF geom = _scrollableWidget->geometry();
    _minX = geom.x();
    _minY = geom.y();
    _maxX = geom.width() - size().width();
    _maxY = geom.height() - size().height();
}

void GenPannableWall::initialize()
{
    _deltaX = 0;
    _deltaY = 0;
    _v  = 0;
    _minX = 0;
    _minY = 0;
    _maxX = 0;
    _maxY = 0;
    _isEasingAngle = false;
    _skipTapGesture = false;
    _enableIndicator = false;

    _scrollingTimeLine = new QTimeLine(MSECS, this);
    _scrollingTimeLine->setCurveShape(QTimeLine::LinearCurve);
    _scrollingTimeLine->setUpdateInterval(30);

    connect(_scrollingTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(scroll(
                                                                            qreal)));
    connect(_scrollingTimeLine, SIGNAL(finished()), this, SLOT(
                scrollingTimelineStoped()));

    _scrollableWidgetRotation = new QGraphicsRotation(this);
    _scrollableWidget->setTransformations(
                QList<QGraphicsTransform*> () << _scrollableWidgetRotation);
    _wallAngle = 0.0;
    recalculateGeometry();
}

void GenPannableWall::rotateWall()
{
    _scrollableWidgetRotation->setAxis((_orientation == Qt::Horizontal) ? Qt::YAxis : Qt::XAxis);
    QPointF origin = _scrollableWidget->mapFromScene(rect().center());
    if (_orientation == Qt::Horizontal)
    {
        origin.setY(_scrollableWidget->rect().center().y());
    }
    else
    {
        origin.setX(_scrollableWidget->rect().center().x());
    }
    _scrollableWidgetRotation->setOrigin(QVector3D(origin));
    _wallAngle = _v * ANGLE_COEF;
    if (_wallAngle > MAX_ANGLE)
        _wallAngle = MAX_ANGLE;
    if (_wallAngle < MIN_ANGLE)
        _wallAngle = MIN_ANGLE;
    QLOG_DEBUG("angle =" << _wallAngle);
    _scrollableWidgetRotation->setAngle(_wallAngle);
}

void GenPannableWall::easeWallAngle()
{
    if (!_isEasingAngle)
    {
        _isEasingAngle = true;
        _scrollableWidgetRotation->setAxis((_orientation == Qt::Horizontal) ? Qt::YAxis : Qt::XAxis);
        QPointF origin = _scrollableWidget->mapFromScene(rect().center());
        if (_orientation == Qt::Horizontal)
        {
            origin.setY(_scrollableWidget->rect().center().y());
        }
        else
        {
            origin.setX(_scrollableWidget->rect().center().x());
        }
        _scrollableWidgetRotation->setOrigin(QVector3D(origin));
        _scrollableWidgetRotation->setAngle(_wallAngle);
        if (!_easeAnim)
        {
            _easeAnim = new QPropertyAnimation(_scrollableWidgetRotation, "angle");
        }
        _easeAnim->setDuration(500);
        _easeAnim->setStartValue(_wallAngle);
        _easeAnim->setEndValue(0);
        _easeAnim->start();
        connect(_easeAnim, SIGNAL(finished()), this, SLOT(finishedEaseAngle()));
        _wallAngle = 0;
    }
}

void GenPannableWall::finishedEaseAngle()
{
    _isEasingAngle = false;
}

void GenPannableWall::scrollingTimelineStoped()
{
    emit emitPositionChanged();
}

Qt::Orientations GenPannableWall::orientation() const
{
    return _orientation;
}

void GenPannableWall::ensureVisible(const QRectF &rectangle)
{
    QRectF rect(mapRectToItem(_scrollableWidget, rectangle));
    QPointF pos = rect.topLeft();
    moveTo(pos);
}

void GenPannableWall::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

#ifdef DRAW_BOUNDARIES
    painter->setPen(Qt::red);
    painter->drawRect(rect());
#endif // DRAW_BOUNDARIES
}

void GenPannableWall::enableScrollIndicator(bool enable)
{
    _enableIndicator = enable;
}

void GenPannableWall::setOrientation(Qt::Orientation orientation)
{
    _orientation = orientation;
    recalculateGeometry();
}

