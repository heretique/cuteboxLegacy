#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsBlurEffect>

#include "genmainwindow.h"
#include "ui_genmainwindow.h"
#include "genbaseview.h"
#include "genconstants.h"
#include "genfeedbackutils.h"
#include "genquerypopup.h"
#include "genwaitpopup.h"
#include "genfileprogress.h"
#include "genpopupmenu.h"
#include "genfilenamepopup.h"
#include "genresourcereader.h"
#include "gennotificationwidget.h"
#include "genutils.h"
#include "genlog.h"

GenMainWindow::GenMainWindow(GenAppModel &model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenMainWindow),
    _appModel(model),
    _currentView(NULL),
    _newView(NULL),
    _animInProgress(false),
    _popupAnimInProgress(false),
    _popupMenu(0),
    _isPopupOpen(false),
    _isProgressOpen(false),
    _isPopupMenuOpen(false),
    _isFilenamePopupOpen(false)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    ui->setupUi(this);
    ui->stackedWidget->setContextMenuPolicy(Qt::NoContextMenu);
    _queryPopup = new GenQueryPopup(this);
    _queryPopup->hide();
    _fileProgressPopup = new GenFileProgress(this);
    _fileProgressPopup->hide();
    _fileNamePopup = new GenFilenamePopup(*menuBar(), _appModel, this);
    _fileNamePopup->hide();
    connect(ui->menuBar, SIGNAL(actionTriggered(const ActionId)),
            SLOT(handleActionTriggered(ActionId)));
    connect(ui->statusBar, SIGNAL(actionTriggered(const ActionId)),
            SIGNAL(actionTriggered(const ActionId)));
    connect(_fileProgressPopup, SIGNAL(fileActionCanceled()),
            SLOT(handleFileActionCanceled()));
    connect(_queryPopup, SIGNAL(popupClosed(ActionId)),
            SLOT(handlePopupClosed(ActionId)));
    connect(_fileNamePopup, SIGNAL(popupClosed(ActionId)),
            SLOT(handleFilenamePopupClosed(ActionId)));
}

GenMainWindow::~GenMainWindow()
{
    delete ui;
    ui = 0;
}

void GenMainWindow::addView(GenBaseView *view)
{
    ui->stackedWidget->addWidget(view);
    connect(view, SIGNAL(triggerAction(ActionId)), SIGNAL(actionTriggered(ActionId)));
}

void GenMainWindow::setDefaultView(GenBaseView *view)
{
    ui->stackedWidget->setCurrentWidget(view);
    _currentView = view;
    _currentView->updateMenuBar(*(ui->menuBar));
}

GenBaseView *GenMainWindow::view(QString className)
{
    GenBaseView *view = NULL;
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QWidget* w = ui->stackedWidget->widget(i);
        if (w->metaObject()->className() == className) {
            view = static_cast<GenBaseView*> (w);
            break;
        }
    }
    return view;
}

GenStatusWidget *GenMainWindow::statusBar()
{
    if (ui)
        return ui->statusBar;
    return NULL;
}

GenFileProgress *GenMainWindow::fileProgressPopup()
{
    return _fileProgressPopup;
}

GenQueryPopup *GenMainWindow::queryPopup()
{
    return _queryPopup;
}

GenMenuBar *GenMainWindow::menuBar()
{
    if (ui)
        return ui->menuBar;
    return NULL;
}

void GenMainWindow::setQueryText(const QString &text)
{
    _queryPopup->setText(text);
}

void GenMainWindow::setQueryAction(const ActionId action)
{
    _queryPopup->setAction(action);
}

bool GenMainWindow::popupOpened()
{
    return (_isProgressOpen || _isPopupOpen || _isFilenamePopupOpen || _isPopupMenuOpen);
}

void GenMainWindow::useImageBackground(bool useImage)
{
    if (ui->stackedWidget)
    {
        if (useImage)
        {
            ui->stackedWidget->setStyleSheet(QString::fromUtf8("background-image: url(:/images/cute_background.png);"));
        }
        else
        {
            ui->stackedWidget->setStyleSheet(QString::fromUtf8("color: rgb(92, 95, 97);\n"
                                                               "background-color: rgb(238, 238, 238);"));
        }
    }
}

void GenMainWindow::showNotificationInfo(QString info, bool autoclose)
{
    GenNotificationWidget *notifWidget = new GenNotificationWidget(this, this);
    notifWidget->setNotificationInfo(info);
    notifWidget->showNotificationInfo(autoclose);
}

void GenMainWindow::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    update();
}

void GenMainWindow::activateView(QString className, const QVariant &data,
                                 bool useAnimation, GenViewAnimType animType)
{
    if (_animInProgress)
        return;

    _currentView = currentView();
    _currentView->aboutToDeactivate();
    _currentView->enableCache();
    _newView = view(className);

    if (!_newView || !_currentView)
        qCritical("View does not exist!!!");

    if (useAnimation && _currentView && _currentView != _newView) {
        _newView->activate(data);
        QAnimationGroup *aniGroup = createViewAnimation(*_currentView, *_newView,
                                                        animType);
        connect(aniGroup, SIGNAL( finished() ), this,
                SLOT( activateAnimationFinished() ));
        connect(aniGroup, SIGNAL( stateChanged(QAbstractAnimation::State,
                                               QAbstractAnimation::State) ), this,
                SLOT( activateAnimationStateChanged(QAbstractAnimation::State,
                                                    QAbstractAnimation::State) ));
        aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
        _animInProgress = true;
        _newView->enableCache();
    }

    if (!useAnimation || _currentView == _newView) {
        _newView->activate(data);
        _newView->enableCache();
        emit animationFinished();
    }
}

void GenMainWindow::activatePreviousView()
{

}

void GenMainWindow::removeView(QString className)
{
    ui->stackedWidget->removeWidget(view(className));
}

GenBaseView* GenMainWindow::currentView()
{
    return static_cast<GenBaseView*> (ui->stackedWidget->currentWidget());
}

QAnimationGroup* GenMainWindow::createViewAnimation(QWidget& currentVisibleView,
                                                    QWidget& nextVisibleView,
                                                    GenViewAnimType animType,
                                                    bool disableDuringAnim)
{
    QRect stackedWidgetRect = ui->stackedWidget->geometry();
    QPoint currentStartPoint = currentVisibleView.geometry().topLeft();
    QPoint currentEndPoint(currentStartPoint);
    int directionX = 0;
    int directionY = 0;
    switch(animType)
    {
    case GenFromLeft:
        directionX = stackedWidgetRect.size().width();
        directionY = 0;
        currentEndPoint.setX(currentEndPoint.x() + directionX);
        break;
    case GenFromTop:
        directionX = 0;
        directionY = -stackedWidgetRect.size().height();
        currentEndPoint.setY(currentEndPoint.y() + directionY);
        break;
    case GenFromBottom:
        directionX = 0;
        directionY = stackedWidgetRect.size().height();
        currentEndPoint.setY(currentEndPoint.y() + directionY);
        break;
    case GenFromRight:
    default:
        directionX = -stackedWidgetRect.size().width();
        directionY = 0;
        currentEndPoint.setX(currentEndPoint.x() + directionX);
        break;
    }

    QLOG_DEBUG("GenMainWindow::createViewAnimation - start pos: " << currentStartPoint);
    QLOG_DEBUG("GenMainWindow::createViewAnimation - end pos: " << currentEndPoint);
    QPropertyAnimation* currentAni = new QPropertyAnimation(
                &currentVisibleView, "pos", this);
    currentAni->setDuration(VIEW_ANIMATION_DURATION);
    currentAni->setStartValue(currentStartPoint);
    currentAni->setEndValue(currentEndPoint);
    currentAni->setEasingCurve(QEasingCurve::InOutCubic);

    QRect nextRect(currentStartPoint + QPoint(-directionX, -directionY),
                   stackedWidgetRect.size());
    nextVisibleView.setGeometry(nextRect);
    nextVisibleView.show();

    QPoint nextStartPoint = nextRect.topLeft();
    QPoint nextEndPoint(currentStartPoint);

    QPropertyAnimation* nextAni = new QPropertyAnimation(&nextVisibleView,
                                                         "pos", this);
    nextAni->setDuration(VIEW_ANIMATION_DURATION);
    nextAni->setStartValue(nextStartPoint);
    nextAni->setEndValue(nextEndPoint);
    nextAni->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(this);
    aniGroup->addAnimation(currentAni);
    aniGroup->addAnimation(nextAni);

    if (disableDuringAnim)
    {
        currentVisibleView.setEnabled(false);
        nextVisibleView.setEnabled(false);
    }

    return aniGroup;
}

QAnimationGroup *GenMainWindow::createPopupShowAnimation(QWidget &popup)
{
    QPropertyAnimation* popupAnim = new QPropertyAnimation(
                &popup, "pos", this);
    popupAnim->setDuration(POPUP_ANIMATION_DURATION);
    popupAnim->setStartValue(ui->menuBar->pos());
    popupAnim->setEndValue(ui->shadowBottom->pos() - QPoint(0, popup.height()));
    popupAnim->setEasingCurve(QEasingCurve::InOutCubic);
    popup.stackUnder(ui->menuBar);
    popup.show();
    QPropertyAnimation* listViewAni = new QPropertyAnimation(ui->stackedWidget,
                                                             "size", this);
    listViewAni->setDuration(POPUP_ANIMATION_DURATION);
    listViewAni->setStartValue(ui->stackedWidget->size());
    QSize endSize = ui->stackedWidget->size();
    endSize.setHeight(endSize.height() - popup.height() - ui->shadowBottom->height());
    listViewAni->setEndValue(endSize);
    listViewAni->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(this);
    aniGroup->addAnimation(popupAnim);
    aniGroup->addAnimation(listViewAni);
    connect(aniGroup, SIGNAL(finished()), SLOT(popupShowAnimationFinished()));
    aniGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
    return aniGroup;
}

QAnimationGroup *GenMainWindow::createPopupHideAnimation(QWidget &popup)
{
    QPropertyAnimation* popupAnim = new QPropertyAnimation(
                &popup, "pos", this);
    popupAnim->setDuration(POPUP_ANIMATION_DURATION);
    popupAnim->setEndValue(ui->menuBar->pos());
    popupAnim->setStartValue(ui->shadowBottom->pos() - QPoint(0, popup.height()));
    popupAnim->setEasingCurve(QEasingCurve::InOutCubic);
    popup.stackUnder(ui->menuBar);
    popup.show();
    QPropertyAnimation* listViewAni = new QPropertyAnimation(ui->stackedWidget,
                                                             "size", this);
    listViewAni->setDuration(POPUP_ANIMATION_DURATION);
    listViewAni->setStartValue(ui->stackedWidget->size());
    QSize endSize = ui->stackedWidget->size();
    endSize.setHeight(ui->shadowBottom->pos().y() + ui->shadowBottom->height() - ui->stackedWidget->pos().y());
    listViewAni->setEndValue(endSize);
    listViewAni->setEasingCurve(QEasingCurve::InOutCubic);

    QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(this);
    aniGroup->addAnimation(popupAnim);
    aniGroup->addAnimation(listViewAni);
    connect(aniGroup, SIGNAL(finished()), SLOT(popupHideAnimationFinished()));
    aniGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
    return aniGroup;
}

void GenMainWindow::activateAnimationFinished()
{
    QLOG_DEBUG("MainWindow::pushAnimationFinished - 1");
    _currentView->disableCache();
    _currentView->setEnabled(true);
    _currentView->deactivate();
    ui->stackedWidget->setCurrentWidget(_newView);
    _newView->disableCache();
    _newView->setEnabled(true);
    _currentView = _newView;
    if (!_isPopupOpen)
        _newView->updateMenuBar(*(ui->menuBar));
    _newView->viewActivated();
    _animInProgress = false;
    emit animationFinished();
}

void GenMainWindow::handlePopupClosed(const ActionId actionId)
{
    _popupAction = actionId;
    hideQueryPopup();
}

void GenMainWindow::handleFilenamePopupClosed(const ActionId actionId)
{
    _popupAction = actionId;
    hideFilenamePopup();
}

void GenMainWindow::handleFileActionCanceled()
{
    hideFileProgressPopup();
    emit actionTriggered(ActionCANCELCURRENTFILEACTION);
}

void GenMainWindow::popupShowAnimationFinished()
{
    _popupAnimInProgress = false;
    _queryPopup->disableCache();
    _fileProgressPopup->disableCache();
    if (_popupMenu)
        _popupMenu->disableCache();
    _fileNamePopup->disableCache();
    _currentView->disableCache();
}

void GenMainWindow::popupHideAnimationFinished()
{
    _popupAnimInProgress = false;
    _queryPopup->disableCache();
    _fileProgressPopup->disableCache();
    if (_popupMenu)
        _popupMenu->disableCache();
    delete _popupMenu;
    _popupMenu = 0;
    _fileNamePopup->disableCache();
    _currentView->disableCache();

    if (_isProgressOpen)
        _isProgressOpen = false;
    if (_isPopupOpen)
    {
        _isPopupOpen = false;
        emit actionTriggered(_popupAction);
    }
    if (_isPopupMenuOpen)
    {
        _isPopupMenuOpen = false;
        QTimer::singleShot(0, this, SLOT(handlePopupMenuCloseSingleShot()));
    }
    if (_isFilenamePopupOpen)
    {
        _isFilenamePopupOpen = false;
        emit actionTriggered(_popupAction);
    }
}

void GenMainWindow::activateAnimationStateChanged(
    QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    QAbstractAnimation::State state = newState;
    state = oldState;
}

void GenMainWindow::handleSettingsClicked()
{
    QApplication::quit();
}

void GenMainWindow::handleButtonTouch()
{
    GenFeedbackUtils::instantFeedback(GenFeedbackUtils::ThemeBasicButton);
}

void GenMainWindow::showPopupMenu(QString caption, QList<QPair<ActionId, QString> > actions)
{
    if (_popupAnimInProgress)
        return;

    if (!popupOpened())
    {
        delete _popupMenu;
        _popupMenu = new GenPopupMenu(this);
        _popupMenu->setCaption(caption);
        _popupMenu->addActions(actions);
        connect(_popupMenu, SIGNAL(actionTriggered(ActionId)),
                SLOT(handleActionTriggered(ActionId)));
        QPropertyAnimation* popupAnim = new QPropertyAnimation(
                    _popupMenu, "pos", this);
        popupAnim->setDuration(POPUP_ANIMATION_DURATION);
        QPoint pos = ui->stackedWidget->pos();
        pos.setX(pos.x() - _popupMenu->width());
        pos.setY(pos.y() + (ui->stackedWidget->height() - _popupMenu->height()) / 2);
        popupAnim->setStartValue(pos);
        pos.setX((ui->stackedWidget->width() - _popupMenu->width()) / 2);
        popupAnim->setEndValue(pos);
        popupAnim->setEasingCurve(QEasingCurve::InOutBack);
        _popupMenu->show();

        connect(popupAnim, SIGNAL(finished()), SLOT(popupShowAnimationFinished()));
        popupAnim->start(QPropertyAnimation::DeleteWhenStopped);
        _popupMenu->enableCache();
        _currentView->enableCache();
        _isPopupMenuOpen = true;
        _popupAnimInProgress = true;
        // set menubar cancel button
        setCancelMenuButton(*menuBar());
        ui->stackedWidget->setEnabled(false);
    }
    else
    {
        qCritical("!!!popup menu already opened!!!");
    }
}

void GenMainWindow::showQueryPopup(const QString& text)
{
    if (_popupAnimInProgress)
        return;

    if (!popupOpened())
    {
        _queryPopup->setText(text);
        createPopupShowAnimation(*_queryPopup);
        _queryPopup->enableCache();
        _currentView->enableCache();
        _isPopupOpen = true;
        _popupAnimInProgress = true;
    }
    else
    {
        qCritical("!!!popup already opened!!!");
    }
}

void GenMainWindow::hidePopupMenu()
{
    if (_popupAnimInProgress)
        return;

    QPropertyAnimation* popupAnim = new QPropertyAnimation(
                _popupMenu, "pos", this);
    popupAnim->setDuration(POPUP_ANIMATION_DURATION);
    popupAnim->setStartValue(_popupMenu->pos());
    QPoint pos = _popupMenu->pos();
    pos.setX(ui->stackedWidget->width());
    popupAnim->setEndValue(pos);
    popupAnim->setEasingCurve(QEasingCurve::InOutBack);
    connect(popupAnim, SIGNAL(finished()), SLOT(popupHideAnimationFinished()));
    popupAnim->start(QPropertyAnimation::DeleteWhenStopped);
    _popupAnimInProgress = true;
    _popupMenu->enableCache();
    _currentView->enableCache();
    ui->stackedWidget->setEnabled(true);
}

void GenMainWindow::hideQueryPopup()
{
    if (_popupAnimInProgress)
        return;

    createPopupHideAnimation(*_queryPopup);
    _popupAnimInProgress = true;
    _queryPopup->enableCache();
    _currentView->updateMenuBar(*(ui->menuBar));
}

void GenMainWindow::hideFilenamePopup()
{
    if (_popupAnimInProgress)
        return;

    createPopupHideAnimation(*_fileNamePopup);
    _popupAnimInProgress = true;
    _fileNamePopup->enableCache();
    _currentView->updateMenuBar(*(ui->menuBar));
}

void GenMainWindow::handlePopupMenuCloseSingleShot()
{
    emit actionTriggered(_popupAction);
}

void GenMainWindow::showFileProgressPopup(const QString& filename)
{
    if (_popupAnimInProgress)
        return;

    if (!popupOpened())
    {
        _fileProgressPopup->setText(filename);
        createPopupShowAnimation(*_fileProgressPopup);
        _fileProgressPopup->reset();
        _fileProgressPopup->start();
        _fileProgressPopup->enableCache();
        _currentView->enableCache();
        _popupAnimInProgress = true;
        _isProgressOpen = true;
    }
}

void GenMainWindow::fileProgress(qint64 bytes, qint64 bytesTotal)
{
    _fileProgressPopup->setItemProgress(bytes, bytesTotal);
}

void GenMainWindow::totalProgress(int current, int total)
{
    _fileProgressPopup->setTotalProgress(current, total);
}

void GenMainWindow::handleActionTriggered(const ActionId actionId)
{
    if (_isPopupOpen)
    {
        if (_queryPopup->handleAction(actionId))
            return;
    }
    if (_isPopupMenuOpen)
    {
        hidePopupMenu();
        if (ActionCANCEL == actionId)
            _currentView->updateMenuBar(*menuBar());
        _popupAction = actionId;
        return;
    }
    if (_isFilenamePopupOpen)
    {
        if (_fileNamePopup->handleAction(actionId))
            return;
    }
    emit actionTriggered(actionId);
}

void GenMainWindow::hideFileProgressPopup()
{
    if (_animInProgress)
        return;

    createPopupHideAnimation(*_fileProgressPopup);
    _fileProgressPopup->enableCache();
    _isPopupOpen = false;
    _popupAnimInProgress = true;
}

void GenMainWindow::showFilenamePopup()
{
    if (_popupAnimInProgress)
        return;

    if (!popupOpened())
    {
        createPopupShowAnimation(*_fileNamePopup);
        _fileNamePopup->enableCache();
        _currentView->enableCache();
        _isFilenamePopupOpen = true;
        _popupAnimInProgress = true;
        _fileNamePopup->updateMenuBar(*menuBar());
    }
    else
    {
        qCritical("!!!popup already opened!!!");
    }
}

void GenMainWindow::showWaitingIndicator(QString text, bool lockInput)
{
    ui->statusBar->showWaitingIndicator(text);
    if (lockInput)
    {
        ui->stackedWidget->setEnabled(false);
        ui->menuBar->setEnabled(false);
    }
}

void GenMainWindow::hideWaitingIndicator()
{
    ui->statusBar->hideWaitingIndicator();
    ui->stackedWidget->setEnabled(true);
    ui->menuBar->setEnabled(true);
}
