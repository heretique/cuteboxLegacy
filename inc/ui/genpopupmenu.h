#ifndef GENPOPUPMENU_H
#define GENPOPUPMENU_H

#include "geninterfaces.h"

namespace Ui {
class GenPopupMenu;
}

class GenPopupMenu : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenPopupMenu(QWidget *parent = 0);
    ~GenPopupMenu();
    void setCaption(QString caption);
    void addActions(QList<QPair<ActionId, QString> > actions);
protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
    void addAction(ActionId actionId, QString actionText, bool separator = true);
signals:
    void actionTriggered(const ActionId actionId);

private:
    Ui::GenPopupMenu *ui;
};

#endif // GENPOPUPMENU_H
