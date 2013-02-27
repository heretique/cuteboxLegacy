#ifndef GENNOTIFICATIONWIDGET_H
#define GENNOTIFICATIONWIDGET_H

#include "geninterfaces.h"

namespace Ui {
    class GenNotificationWidget;
}

class GenMainWindow;

class GenNotificationWidget : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenNotificationWidget(GenMainWindow *mainWindow, QWidget *parent = 0);
    ~GenNotificationWidget();

    void setNotificationInfo(QString info);
    void showNotificationInfo(bool autoclose = true);

protected:
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *);

public slots:
    void closeNotification();

protected slots:
    void notifCloseAnimFinished();
    void notifOpenAnimFinished();

private:
    Ui::GenNotificationWidget *ui;
    GenMainWindow *_mainWindow;
    bool _animInprogress;
};

#endif // GENNOTIFICATIONWIDGET_H
