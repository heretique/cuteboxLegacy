#include <QAbstractScrollArea>
#include <QPixmapCache>
#include <QPainter>

#include "genkineticscrollwidget.h"
#include "genkineticscrollengine.h"
#include "genlog.h"

GenKineticScrollWidget::GenKineticScrollWidget(QWidget *parent) :
    GenCachedWidget(parent), _kineticEngine(NULL)
{

}

GenKineticScrollWidget::~GenKineticScrollWidget()
{
    delete _kineticEngine;
}

void GenKineticScrollWidget::enableKineticScrollingFor(
    QAbstractScrollArea *scrollArea, Qt::Orientation orientation)
{
    if (_kineticEngine)
    {
        delete _kineticEngine;
        _kineticEngine = NULL;
    }

    _kineticEngine = new GenKineticScrollEngine(orientation, this);

    _kineticEngine->enableKineticScrollFor(scrollArea);
}

const GenKineticScrollEngine *GenKineticScrollWidget::kineticEngine() const
{
    return _kineticEngine;
}

void GenKineticScrollWidget::resizeEvent(QResizeEvent *event)
{
    if (_kineticEngine)
    {
        _kineticEngine->adjustSize();
    }
}
