#ifndef PANNABLEWIDGET_H_
#define PANNABLEWIDGET_H_

#include <QGraphicsWidget>
#include <QPainterPath>

/*!
 * \class PannableWidget
 * \class PannableWidget
 * \brief PannableWidget is just a simple wrapper which holds the layout for the
 * pannable widgets.
 */
class GenPannableWidget : public QGraphicsWidget
{
public:
    //! PannableWidget constructor
    //
    //! \param parent optional parent
    GenPannableWidget(Qt::Orientation orientation, QGraphicsObject *parent = 0);

    //! Destructor
    virtual ~GenPannableWidget();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);

    //! Shape method in order to make children to clip to shape
    //! \return instance shape in QPainterPath object.
    QPainterPath shape() const;

    //! Set a widget for this widget
    void addWidget(QGraphicsWidget *widget, int pos = 0);

    void addItem(QGraphicsLayoutItem *item, int pos = 0);

    //! Set a widget for this widget
//    void removeWidget(QGraphicsWidget *widget);

    /*!
     * \brief Removes the widget at the given position
     * \param pos widget's position in list(layout)
     */
    void removeAt(int pos);

    /*!
     * \brief Retrieves the widget at the given position
     * \param pos widget's position in list(layout)
     */
    QGraphicsWidget* itemAt(int pos);

    /*!
     * \brief Remove all items from widget's layout
     */
    virtual void removeAllItems(bool destroy);

    /*!
     * \brief Returns the number of items
     * \return int
     */
    int count() const;

    /*!
     * \brief Sets the contents margins for layout
     * \param left, int argument.
     * \param top, int argument.
     * \param right, int argument.
     * \param bottom, int argument.
     */
    void setContentsMargins(int left, int top, int right, int bottom);

    /*!
     * \brief Called when long tap occured on view.
     * \param pos The position where the long tap occured.
     */
    void longTap(QPointF &pos);

    /*!
     * \brief Called when tap occured on view.
     * \param pos The position where the tap occured.
     */
    void tap(const QPointF &pos);
};

#endif /* PANNABLEWIDGET_H_ */
