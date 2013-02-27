#ifndef GENFILEACTIONSPOPUP_H
#define GENFILEACTIONSPOPUP_H

#include "geninterfaces.h"

namespace Ui {
    class GenQueryPopup;
}

class GenPushButton;

class GenQueryPopup : public GenCachedWidget, public GenControllerInterface
{
    Q_OBJECT

public:
    explicit GenQueryPopup(QWidget *parent = 0);
    ~GenQueryPopup();
    void setText(const QString& text);
    void setAction(const ActionId actionId);
    const ActionId action() const;

public slots:
    bool handleAction(const ActionId actionId);

signals:
    void popupClosed(const ActionId actionId);

private:
    Ui::GenQueryPopup *ui;
    ActionId _actionId;
};

#endif // GENFILEACTIONSPOPUP_H
