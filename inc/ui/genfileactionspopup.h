#ifndef GENFILEACTIONSPOPUP_H
#define GENFILEACTIONSPOPUP_H

#include "geninterfaces.h"

namespace Ui {
    class GenFileActionsPopup;
}

class GenPushButton;

class GenFileActionsPopup : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenFileActionsPopup(QWidget *parent = 0);
    ~GenFileActionsPopup();
    const GenPushButton *copyButton();
    const GenPushButton *moveButton();
    const GenPushButton *deleteButton();
    const GenPushButton *uploadDownloadButton();

signals:
    void actionTriggered(const ActionId actionId);

private:
    Ui::GenFileActionsPopup *ui;
};

#endif // GENFILEACTIONSPOPUP_H
