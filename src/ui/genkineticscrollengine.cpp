#include <QPoint>
#include <QRect>
#include <QTimeLine>
#include <math.h>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QList>
#include <QAbstractScrollArea>
#include <QEvent>
#include <QApplication>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLineEdit>

// Only for testing.
#include <QCursor>

#include "genkineticscrollengine.h"
#include "genfeedbackutils.h"
#include "genlog.h"

#define PANNABLE_FRICTION_COEFFICIENT 0.15 * 9.81  // F = y*N, 0.1 = friction coefficient for ice on ice.
#define TAP_GESTURE_THRESHOLD 15
#define VELOCITY_THRESHOLD 0.5
#define MSECS 1000
#define SCROLL_MIN_THRESHOLD 0
#define SCROLL_MAX_THRESHOLD 0.35
#define SLOW_PERCENT -0.4
#define SWIPE_GESTURE_THRESHOLD 60


GenKineticScrollEngine::GenKineticScrollEngine(Qt::Orientation orientation,
                                               QWidget *parent) :
    QWidget(parent), _scrollArea(NULL), _orientation(orientation)
{
    initialize();
}

GenKineticScrollEngine::~GenKineticScrollEngine()
{

}

void GenKineticScrollEngine::enableKineticScrollFor(
    QAbstractScrollArea* scrollArea)
{
    if (!scrollArea)
    {
        Q_ASSERT_X(0, "kinetic scroller", "missing scroll area");
        return;
    }

    _scrollArea = scrollArea;

    connect(_scrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int, int)),
            this, SLOT(adjustSize(int, int)));
    connect(_scrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)),
            this, SLOT(adjustSize(int, int)));
    connect(_scrollArea->verticalScrollBar(), SIGNAL(sliderPressed()), this,
            SLOT(stopKineticScrolling()));
    connect(_scrollArea->horizontalScrollBar(), SIGNAL(sliderPressed()), this,
            SLOT(stopKineticScrolling()));

    setScrollable(true);

    adjustSize();
}

void GenKineticScrollEngine::setScrollable(bool scrollable)
{
    if (!scrollable)
    {
        stopScrolling();
        moveToBegin();
    }
    _scrollable = scrollable;
}

void GenKineticScrollEngine::adjustSize(int min, int max)
{
    Q_UNUSED(min)
    Q_UNUSED(max)

    stopScrolling();

    if (_scrollArea)
    {
        QRect geom = _scrollArea->geometry();

//        //Vertical scrollbar
//        QScrollBar *scrollBar = _scrollArea->verticalScrollBar();

//        if (_scrollArea->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff
//                && scrollBar->minimum() != scrollBar->maximum())
//        {
//            geom.setWidth(geom.width() - scrollBar->width());
//        }

//        //Horizontal scrollbar
//        scrollBar = _scrollArea->horizontalScrollBar();

//        if (_scrollArea->horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff
//                && scrollBar->minimum() != scrollBar->maximum())
//        {
//            geom.setHeight(geom.height() - scrollBar->height());
//        }

        setGeometry(geom);
    }
    else
    {
        setGeometry(parentWidget()->rect());
    }
}

void GenKineticScrollEngine::mousePressEvent(QMouseEvent *event)
{
    emit
            stopAnimation();

    if (event->button() != Qt::LeftButton)
    {
        forwardMouseEvent(event);
        return;
    }


    if (_scrollingTimeLine->state() == QTimeLine::Running)
    {
        stopScrolling(false);
        _skipTapGesture = true; // We don't want to stop and tap. We want stop or tap
    }

    _gestureTime.start();

    _oldMousePos = event->pos();
    _mousePressPos = event->pos();
    GenFeedbackUtils::instantFeedback(GenFeedbackUtils::ThemeBasicItem);
}

void GenKineticScrollEngine::mouseMoveEvent(QMouseEvent *event)
{

    if (_oldMousePos.x() < 0 && _oldMousePos.y() < 0)
    {
        _oldMousePos = event->pos();
    }

    int deltaX = 0;
    int deltaY = 0;

    QPoint delta = event->pos() - _oldMousePos;
    if (_orientation & Qt::Horizontal)
    {
        deltaX = delta.x();
    }

    if (_orientation & Qt::Vertical)
    {
        deltaY = delta.y();
    }

    _oldMousePos = event->pos();

    // translate the view by delta
    scrollBy(deltaX, deltaY);
}

void GenKineticScrollEngine::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        forwardMouseEvent(event);
        return;
    }

    QPoint pressPos = _mousePressPos;
    QPoint releasePos = event->pos();

    QPoint delta = pressPos - releasePos;
    qreal distance =
            sqrt(pow((float) delta.x(), 2) + pow((float) delta.y(), 2));

    _oldMousePos = QPoint(-1, -1);

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

QWidget *GenKineticScrollEngine::forwardMouseEvent(QMouseEvent *event)
{
    QWidget *item = _scrollArea->childAt(event->pos());

    if (item)
    {
        QPoint nPos = item->mapFromGlobal(event->globalPos());

        QMouseEvent *mouseEvent = new QMouseEvent(event->type(), nPos,
                                                  event->button(), event->buttons(), event->modifiers());

        QApplication::postEvent(item, mouseEvent);
    }

    return item;
}

void GenKineticScrollEngine::scrollBy(int x, int y)
{
    int oldx = _scrollArea->horizontalScrollBar()->value();
    int oldy = _scrollArea->verticalScrollBar()->value();
    _scrollArea->verticalScrollBar()->setValue(oldy - y);
    _scrollArea->horizontalScrollBar()->setValue(oldx - x);
}

void GenKineticScrollEngine::scrollTo(int x, int y)
{
    _scrollArea->verticalScrollBar()->setValue(y);
    _scrollArea->horizontalScrollBar()->setValue(x);
}

void GenKineticScrollEngine::startScrolling(int duration)
{
    _scrollingTimeLine->setCurrentTime(0);
    _scrollingTimeLine->setDuration(duration);
    _scrollingTimeLine->start();
}

void GenKineticScrollEngine::stopScrolling(bool clear)
{
    _scrollingTimeLine->stop();

    if (clear)
    {
        _scrollingTimeLine->setCurrentTime(0);
        _vX = 0;
        _vY = 0;
    }

    emitPositionChanged();
}

bool GenKineticScrollEngine::tap(QMouseEvent *event)
{
    GenFeedbackUtils::instantFeedback(GenFeedbackUtils::ThemeBasicItem);

    endReached();

    if (!(event->type() == QEvent::MouseButtonPress || event->type()
          == QEvent::MouseButtonRelease))
    {
        return false;
    }

    if (event->button() != Qt::LeftButton)
    {
        return false;
    }

    QMouseEvent* mousePress = new QMouseEvent(QEvent::MouseButtonPress,
                                              event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    forwardMouseEvent(mousePress);
    QWidget *receiver = forwardMouseEvent(event);

    //This is the nasty part
    //Need to explicitely set focus on input widgets
    if (qobject_cast<QTextEdit*> (receiver) || qobject_cast<QPlainTextEdit*> (
                receiver) || qobject_cast<QLineEdit*> (receiver))
    {
        receiver->setFocus(Qt::MouseFocusReason);
    }
    else
    {
        setFocus();
    }

    emit
            tapped(event->pos());

    return true;
}

bool GenKineticScrollEngine::swipe(QPoint const & startPoint,
                                   QPoint const &endPoint)
{
    _skipTapGesture = false;
    QPoint delta = endPoint - startPoint;
    qint32 tD = _gestureTime.elapsed(); // Delta Time
    qreal dxD = delta.x(); // Delta X distance
    qreal dyD = delta.y(); // Delta Y distance


    qreal vx = (dxD / tD) * MSECS; // Velocity in x dimension (pixels / second).
    qreal vy = (dyD / tD) * MSECS; // Velocity in y dimension (pixels / second).

    // Store swipe style for later use
    SwipeDirection direction = storeSwipeStyle(startPoint, endPoint);
    QLOG_DEBUG("SWIPE_STYLE: " << _swipeStyle);
    QLOG_DEBUG("SWIPE_DIRECTION: " << direction);

    if (_swipeStyle != Diagonal)
        emit flicked(direction);

    if (fabs(vx) > VELOCITY_THRESHOLD || fabs(vy) > VELOCITY_THRESHOLD)
    {
        qreal velocity = 0; // Current velocity in x or y dimension.

        _deltaX = dxD;
        _deltaY = dyD;

        _vX = vx;
        _vY = vy;

        if (_orientation & Qt::Horizontal)
        {
            velocity = fabs(vx);
        }
        else
        {
            velocity = fabs(vy);
        }

        if (velocity > 0 && !endReached())
        {
            // Actually this duration doesn't effect on anything at the moment.
            startScrolling(static_cast<int> (velocity
                                             / PANNABLE_FRICTION_COEFFICIENT));
        }
    }
    return true;
}

void GenKineticScrollEngine::scroll(qreal value)
{
    Q_UNUSED(value)
    // Check first if we have reached one of the ends
    if (endReached())
    {
        return;
    }

    qreal sX = 0, sY = 0;

    Qt::Orientations orientation = _orientation;
    if (_orientation & Qt::Vertical && _orientation & Qt::Horizontal)
    {
        switch (_swipeStyle)
        {
        case Vertical:
            orientation = Qt::Vertical;
            break;
        case Horizontal:
            orientation = Qt::Horizontal;
            break;
        default:
            break;
        }
    }

    if (orientation & Qt::Horizontal)
    {
        // s = 1/2 * (u + v)t,
        // u=0,
        // t=update interval (in ms) of the QTimeLine.
        sX = _vX * (_scrollingTimeLine->updateInterval() / 2) / MSECS;

        // v = u + at,
        // u = d_ptr->m_vX (current speed),
        // a = yN/m, m = 1kg (just to have something), yN = friction
        qreal v = (fabs(_vX) - (PANNABLE_FRICTION_COEFFICIENT
                                * _scrollingTimeLine->updateInterval()));
        _vX = (_vX < 0 ? -v : v);

        if (fabs(sX) > SCROLL_MIN_THRESHOLD && fabs(sX) < SCROLL_MAX_THRESHOLD)
        {
            stopScrolling();
        }
    }

    if (orientation & Qt::Vertical)
    {
        // s = 1/2 * (u + v)t,
        // u=0,
        // t=update interval (in ms) of the QTimeLine.
        sY = _vY * (_scrollingTimeLine->updateInterval() / 2) / MSECS;

        // v = u + at,
        // u = d_ptr->m_vY (current speed),
        // a = yN/m, m = 1kg (just to have something), yN = friction
        qreal v = (fabs(_vY) - (PANNABLE_FRICTION_COEFFICIENT
                                * _scrollingTimeLine->updateInterval()));
        _vY = (_vY < 0 ? -v : v);

        if (fabs(sY) > SCROLL_MIN_THRESHOLD && fabs(sY) < SCROLL_MAX_THRESHOLD)
        {
            stopScrolling();
        }
    }


    scrollBy(sX * 2, sY * 2);

    emitPositionChanged();
}

GenKineticScrollEngine::SwipeDirection GenKineticScrollEngine::storeSwipeStyle(QPoint const &first,
                                                                               QPoint const &second)
{
    qreal yD = second.y() - first.y();
    qreal xD = second.x() - first.x();

    SwipeDirection direction = Unknown;

    if (fabs(xD) > SWIPE_GESTURE_THRESHOLD && fabs(xD) > fabs(yD))
    {
        _swipeStyle = Horizontal;
        if (xD > 0)
            direction = Right;
        else
            direction = Left;
    }

    if (fabs(yD) > SWIPE_GESTURE_THRESHOLD && fabs(yD) > fabs(xD))
    {
        _swipeStyle = Vertical;
        if (yD > 0)
            direction = Bottom;
        else
            direction = Top;
    }
    return direction;
}

bool GenKineticScrollEngine::endReached()
{
    QScrollBar *srollBar = _scrollArea->verticalScrollBar();

    bool verticalEndReached = _orientation & Qt::Vertical && (srollBar->value()
                                                              >= srollBar->maximum() || srollBar->value() <= srollBar->minimum());

    srollBar = _scrollArea->horizontalScrollBar();

    bool horizontalEndReached = _orientation & Qt::Horizontal
            && (srollBar->value() >= srollBar->maximum() || srollBar->value()
                <= srollBar->minimum());

    return verticalEndReached || horizontalEndReached;
}

void GenKineticScrollEngine::emitPositionChanged()
{
    int vx = _scrollArea->horizontalScrollBar()->value();
    int vy = _scrollArea->verticalScrollBar()->value();
    QPoint point = QPoint(vx, vy);
    //TODO: might not be correct
    emit posChanged(QRect(point, _scrollArea->rect().size()));
}

void GenKineticScrollEngine::stopKineticScrolling()
{
    stopScrolling();
}

void GenKineticScrollEngine::moveToBegin()
{
    scrollTo(_scrollArea->horizontalScrollBar()->minimum(),
             _scrollArea->verticalScrollBar()->minimum());
}

void GenKineticScrollEngine::moveToEnd()
{
    scrollTo(_scrollArea->horizontalScrollBar()->maximum(),
             _scrollArea->verticalScrollBar()->maximum());
}

bool GenKineticScrollEngine::isScrolling() const
{
    return (_scrollingTimeLine->state() == QTimeLine::Running);
}


void GenKineticScrollEngine::initialize()
{
    _scrollable = false;

    _deltaX = 0;
    _deltaY = 0;
    _vX = 0;
    _vY = 0;
    _skipTapGesture = false;
    _enableContinuousScrolling = false;

    _scrollingTimeLine = new QTimeLine(MSECS, this);
    _scrollingTimeLine->setCurveShape(QTimeLine::LinearCurve);
    _scrollingTimeLine->setUpdateInterval(30);

    connect(_scrollingTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(scroll(
                                                                            qreal)));
    connect(_scrollingTimeLine, SIGNAL(finished()), this, SLOT(
                emitPositionChanged()));
}

void GenKineticScrollEngine::enableContinuousScrolling(bool enable)
{
    _enableContinuousScrolling = enable;
}

Qt::Orientations GenKineticScrollEngine::orientation() const
{
    return _orientation;
}

void GenKineticScrollEngine::setOrientation(Qt::Orientation orientation)
{
    _orientation = orientation;
}
