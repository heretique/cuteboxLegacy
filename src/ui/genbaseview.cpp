#include "genbaseview.h"
#include "genmainwindow.h"

#include <QApplication>
#include <QPixmapCache>

GenBaseView::GenBaseView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenCachedWidget(parent), _mainWindow(mainWindow), _appModel(appModel)
{
    setContextMenuPolicy(Qt::NoContextMenu);
}

GenBaseView::~GenBaseView()
{

}

void GenBaseView::activateView(QString className, const QVariant &data,
    bool useAnim, GenViewAnimType animType)
{
    _mainWindow->activateView(className, data, useAnim, animType);
}

void GenBaseView::setViewData(const QVariant &data)
{

}

void GenBaseView::activatePreviousView()
{
    _mainWindow->activatePreviousView();
}

void GenBaseView::viewActivated()
{

}

void GenBaseView::aboutToDeactivate()
{

}

void GenBaseView::exit()
{
    QApplication::exit(0);
}
