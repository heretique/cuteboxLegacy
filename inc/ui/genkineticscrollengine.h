#ifndef PANNABLEVIEW_HH
#define PANNABLEVIEW_HH

#include <QWidget>
#include <QTimeLine>
#include <QTime>
#include <QTimer>

class QTimeLine;
class QMouseEvent;
class QAbstractScrollArea;

class GenKineticScrollEngine : public QWidget
{
Q_OBJECT

public:
    enum SwipeStyle
    {
        Horizontal = 0,
        Vertical,
        Diagonal
    };

    enum SwipeDirection
    {
        Unknown = 0,
        Left,
        Right,
        Top,
        Bottom
    };

    /*!
     * \brief Constructor
     * \param orientation Qt::Horizontal or Qt::Vertical.
     * \param parent An optional parent
     */
    GenKineticScrollEngine(Qt::Orientation orientation, QWidget *parent = 0);

    /*!
     * \brief Destructor
     */
    virtual ~GenKineticScrollEngine();

    /*!
     * \brief Enables kinetic scrolling for scrollArea
     * \param scrollArea, a QAbstractScrollArea* argument. The target scroll area.
     */
    void enableKineticScrollFor(QAbstractScrollArea* scrollArea);

    /*!
     * \brief Enables/disables scrolling
     */
    void setScrollable(bool scrollable);

public slots:
    /*!
     * \brief Adjusts the kietic engine's surface to cover the destination scroll area 
     */
    void adjustSize(int min = 0, int max = 0);

protected:
    /*!
     * \brief Reimplemented from QWidget
     */
    void mousePressEvent(QMouseEvent *event);

    /*!
     * \brief Reimplemented from QWidget
     */
    void mouseMoveEvent(QMouseEvent *event);

    /*!
     * \brief Reimplemented from QWidget
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /*!
     * \brief Forwards a mouse event to the underlying top level widget
     * \param event a QMouseEvent* argument
     */
    QWidget *forwardMouseEvent(QMouseEvent *event);

    /*!
     * \brief Scrolls by x and y
     * \param x, an int argument. The vertical scroll offet.
     * \param y, an int argument. The horizontal scroll offet.
     */
    void scrollBy(int x, int y);

    /*!
     * \brief Scrolls to a specified point
     * \param x, an int argument. The vertical scroll offet.
     * \param y, an int argument. The horizontal scroll offet.
     */
    void scrollTo(int x, int y);

    /*!
     * \brief Returns true if view is scrolling, false otherwise
     * \return bool
     */
    bool isScrolling() const;

    /*!
     * \brief Move current position to the begin
     */
    void moveToBegin();

    /*!
     * \brief Move current position to the end
     */
    void moveToEnd();

    //! Enable continous scrolling. This  means that when one of the ends of the pannable
    //! widget has been reached the widget moves to the opposite end.
    //! \param enable True if enabling continuous scrolling, false disables it.
    void enableContinuousScrolling(bool enable);

    //! \return The panning orientation.
    Qt::Orientations orientation() const;

public slots:
    /*!
     * \brief Sets the scroll direction.
     * \param orientation Qt::Vertical or Qt::Horizontal
     */
    void setOrientation(Qt::Orientation orientation);

protected:
    /*!
     * \brief Start scrolling
     * \param duration, an int argument. Scroll duration time in milliseconds.
     */
    void startScrolling(int duration = 1000000);

    /*!
     * \brief Stop scrolling.
     * \param clear, an int argument. Specified if scrolling state is to be reset.
     */
    void stopScrolling(bool clear = true);

    /*!
     * \brief Handle tapping.
     * \param event. The event.
     *  \return true if success, false otherwise
     */
    bool tap(QMouseEvent * event);

    /*!
     * \brief Handle swipe gesture.
     * \param startPoint. The point where the mouse was pressed
     * \param endPoint. The point where the mouse was released
     * \return true on success, false on failure
     */
    bool swipe(QPoint const & startPoint, QPoint const &endPoint);

    /*!
     * \brief Stores the swipe direction
     * \param first, a QPoint argument. The first point.
     * \param second, a QPoint argument. The second point.
     */
    GenKineticScrollEngine::SwipeDirection storeSwipeStyle(QPoint const &first, QPoint const &second);

    /*!
     * \brief Checks if an end was reached.
     *  \return true if end has been reached, return false otherwise.
     */
    bool endReached();

    /*!
     * \brief Minimum and maximum, size related parameters
     */
    void recalculateGeometry();

    /*!
     * \brief Initializes the engine
     */
    void initialize();

protected slots:
    /*!
     * \brief A slot for scrolling.
     */
    void scroll(qreal value);

    /*!
     * \brief A slot for emitting posChanged() signal of parent
     */
    void emitPositionChanged();
    
    /*!
     * \brief Stops the kinetic scrolling
     * \detail Called when the user starts scrolling using the scroll bar
     */
    void stopKineticScrolling();

signals:
    /*!
     * \brief This signal is emitted when position of the pannable item has been changed.
     * \detail The density of how often this signal is emitted can 
     * be changed with \see setPosChangedInterval(int interval
     * \param viewRect The rectangle of the viewport.
     */
    void posChanged(QRect const & viewRect);

    //! 
    //! 
    /*!
     * \brief A signal for indicating tapping.
     * \param point. The tapping position.
     */
    void tapped(QPoint const &point);

    /*!
     * \brief Signal for stopping the animation
     */
    void stopAnimation();

    void flicked(GenKineticScrollEngine::SwipeDirection direction);

private:
    QAbstractScrollArea *_scrollArea; /*!< The destination scroll area */
    bool _scrollable; /*!< Indicates whether scrolling is enabled */
    bool _enableContinuousScrolling; /*!< Flag for enabling/disabling continuous scrolling */
    Qt::Orientation _orientation; /*!< Scroll direction */
    qreal _deltaX; /*!< Delta X */
    qreal _deltaY; /*!< Delta Y */
    qreal _vX; /*!< Calculated horizontal speed */
    qreal _vY; /*!< Calculated vertical speed */
    QTime _gestureTime; /*!< Timer for capturing time interval between press and release */
    QTimeLine *_scrollingTimeLine; /*!< Scrolling time line */
    bool _skipTapGesture; /*!< Flag for skipping tap gesture when tapping durind scrolling animation */
    SwipeStyle _swipeStyle; /*!< Swipe gesture direction */
    QPoint _oldMousePos; /*!< Last recorded mouse position */
    QPoint _mousePressPos; /*!< Last recorded mouse pressing position */
};

#endif
