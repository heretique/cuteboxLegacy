#ifndef KINETICSCROLLWIDGET_H_
#define KINETICSCROLLWIDGET_H_

#include "geninterfaces.h"

class QAbstractScrollArea;
class GenKineticScrollEngine;

class GenKineticScrollWidget : public GenCachedWidget
{
public:
    /*!
     * \brief Constructor
     * \param parent, a QWidget* argument. The parent widget.
     */
    GenKineticScrollWidget(QWidget *parent = 0);

    /*!
     * \brief Destructor
     */
    virtual ~GenKineticScrollWidget();

    /*!
     * \brief Enables kinetic scrolling for the specified widget
     * \detail For vertical or horizontal scrolling, the respective scroll bars must be enabled.
     * \param scrollArea, a QAbstractScrollArea* argument. The target scroll area.
     * \param orientation, an Orientation argument. Specifies vertical or horizontal scrolling
     */
    void enableKineticScrollingFor(QAbstractScrollArea *scrollArea,
            Qt::Orientation orientation);

    const GenKineticScrollEngine *kineticEngine() const;
protected:
    /*!
     * \brief Reimplemented from QWidget
     */
    void resizeEvent(QResizeEvent *event);
private:
    GenKineticScrollEngine *_kineticEngine; /*!< Kinetic engine */
};

#endif /* GenKineticScrollWidget_H_ */
