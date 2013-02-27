#ifndef INTERFACES_H
#define INTERFACES_H

#include <QVariant>
#include <QWidget>
#include "gentypes.h"

class GenMenuBar;

class GenViewInterface
{
public:
    /*!
     * \brief Activates the view
     * \param data, a QVariant argument. Additional data.
     */
    virtual void activate(const QVariant &data = QVariant()) = 0;

    /*!
     * \brief Deactivates the view
     */
    virtual void deactivate() = 0;

};

class GenControllerInterface
{
public:
    virtual bool handleAction(const ActionId actionId) = 0;
};

class GenMenuBarController
{
public:
    virtual void updateMenuBar(GenMenuBar &menuBar) = 0;
};


class GenCachedWidget : public QWidget
{
    Q_OBJECT
public:
    GenCachedWidget(QWidget *parent = 0);
    ~GenCachedWidget();

public slots:
    void enableCache();
    void disableCache();

protected:
    void paintEvent(QPaintEvent *e);
};

class GenTapHandlingInterface
{
public:

    /*!
     * \brief Destructor
     */
    virtual ~GenTapHandlingInterface();

    /*!
     * \brief derived classes implement this method for handling the tap event
     */
    virtual void handleTap(const QPointF& scenePos) = 0;
    /*!
     * \brief derived classes implement this method for handling the long tap event
     * \param position, a const QPointF& arg. The position of the event.
     */
    virtual void handleLongTap(const QPointF& scenePos) = 0;

};

#endif // INTERFACES_H
