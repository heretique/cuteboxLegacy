#include <QPropertyAnimation>
#include <QTimer>
#include <QResizeEvent>

#include "gennotificationwidget.h"
#include "ui_gennotificationwidget.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genmenubar.h"
#include "genconstants.h"

GenNotificationWidget::GenNotificationWidget(GenMainWindow *mainWindow, QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenNotificationWidget),
    _mainWindow(mainWindow),
    _animInprogress(false)
{
    ui->setupUi(this);
    hide();
}

GenNotificationWidget::~GenNotificationWidget()
{
    delete ui;
}

void GenNotificationWidget::setNotificationInfo(QString info)
{
    ui->infoLabel->setText(info);
    resize(QSize(360, ui->verticalLayout->sizeHint().height()));
}

void GenNotificationWidget::showNotificationInfo(bool autoclose)
{
    QPropertyAnimation* notificationAnim = new QPropertyAnimation(this, "pos", this);
    notificationAnim->setDuration(NOTIFICATION_ANIMATION_DURATION);
    QPoint startPos = _mainWindow->statusBar()->geometry().bottomLeft();
    startPos.setY(startPos.y() - height());
    notificationAnim->setStartValue(startPos);
    notificationAnim->setEndValue(_mainWindow->statusBar()->geometry().bottomLeft());
    notificationAnim->setEasingCurve(QEasingCurve::InOutCubic);
    stackUnder(_mainWindow->statusBar());
    show();
    enableCache();
    connect(notificationAnim, SIGNAL(finished()), SLOT(notifOpenAnimFinished()));
    notificationAnim->start(QPropertyAnimation::DeleteWhenStopped);
    _animInprogress = true;
    if (autoclose)
        QTimer::singleShot(NOTIFICATION_AUTOCLOSE_TIME, this, SLOT(closeNotification()));
}

void GenNotificationWidget::resizeEvent(QResizeEvent *e)
{
    if (ui)
    {
        ui->verticalLayoutWidget->resize(QSize(360, e->size().height()));
    }
    QWidget::resizeEvent(e);
}

void GenNotificationWidget::mousePressEvent(QMouseEvent *)
{
    if (!_animInprogress)
        closeNotification();
}


void GenNotificationWidget::closeNotification()
{
    QPropertyAnimation* notificationAnim = new QPropertyAnimation(this, "pos", this);
    notificationAnim->setDuration(NOTIFICATION_ANIMATION_DURATION);
    QPoint startPos = _mainWindow->statusBar()->geometry().bottomLeft();
    notificationAnim->setStartValue(startPos);
    startPos.setY(startPos.y() - height());
    notificationAnim->setEndValue(startPos);
    notificationAnim->setEasingCurve(QEasingCurve::InOutCubic);
    stackUnder(_mainWindow->statusBar());
    connect(notificationAnim, SIGNAL(finished()), SLOT(notifCloseAnimFinished()));
    notificationAnim->start(QPropertyAnimation::DeleteWhenStopped);
    _animInprogress = true;
}

void GenNotificationWidget::notifCloseAnimFinished()
{
    _animInprogress = false;
    hide();
    deleteLater();
}

void GenNotificationWidget::notifOpenAnimFinished()
{
    _animInprogress = false;
}
