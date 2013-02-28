#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <QWidget>
#include <QVariant>
#include <QPainter>

#include "genenums.h"
#include "gentypes.h"
#include "geninterfaces.h"

class GenMainWindow;
class GenAppModel;
class GenMenuBar;

class GenBaseView: public GenCachedWidget,
        public GenViewInterface,
        public GenControllerInterface,
        public GenMenuBarController
{
    Q_OBJECT
public:
    GenBaseView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent);
    /*!
     * \brief Destructor
     */
    virtual ~GenBaseView();


    // Activates existing view
    virtual void activateView(QString className, const QVariant &data =
            QVariant(), bool useAnimation = true, GenViewAnimType animType = GenFromRight);
    virtual void setViewData(const QVariant &data = QVariant());

public Q_SLOTS:
    virtual void exit();

    // Activates pervious view (back) and deletes current
    virtual void activatePreviousView();
    
    virtual void viewActivated();
    virtual void aboutToDeactivate();

signals:
    void triggerAction(ActionId action);

protected:
    GenMainWindow *_mainWindow;
    GenAppModel *_appModel;
};

#endif // BASEVIEW_H
