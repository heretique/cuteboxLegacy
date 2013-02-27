#ifndef GENPUSHBUTTON_H
#define GENPUSHBUTTON_H

#include <QPushButton>
#include "gentypes.h"
#include "genconstants.h"

class GenPushButton : public QPushButton
{
    Q_OBJECT
public:
    GenPushButton(QWidget *parent = NULL, const ActionId actionId = NOID);
    ~GenPushButton();

    void setAction(const ActionId actionId);
    ActionId actionId() const;

signals:
    void clicked(const ActionId actionId);

private slots:
    void handleClicked();
    void handlePressed();
    void handleReleased();

private:
    ActionId _actionId;
};

#endif // GENPUSHBUTTON_H
