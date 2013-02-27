#ifndef GENMENUBAR_H
#define GENMENUBAR_H

#include <QWidget>
#include "genconstants.h"

namespace Ui {
    class GenMenuBar;
}

class GenMenuBar : public QWidget
{
    Q_OBJECT
public:
    enum Button
    {
        GenMenuButtonsNone = 0x0,
        GenMenuButton1 = 0x01,
        GenMenuButton2 = 0x02,
        GenMenuButton3 = 0x04,
        GenMenuButton4 = 0x08,

        GenMenuAllButtons = 0xff
    };
    Q_DECLARE_FLAGS(Buttons, Button)
    explicit GenMenuBar(QWidget *parent = 0);
    ~GenMenuBar();

    void setMenuActions(const ActionId action1,
                        const ActionId action2 = ActionNONE,
                        const ActionId action3 = ActionNONE,
                        const ActionId action4 = ActionNONE);
    void setMenuIcons(QPixmap icon1,
                      QPixmap icon2 = QPixmap(),
                      QPixmap icon3 = QPixmap(),
                      QPixmap icon4 = QPixmap());
    void setMenuButtons( GenMenuBar::Buttons buttons,
                        GenMenuBar::Buttons enabledButtons = GenMenuBar::GenMenuAllButtons);
    void setMenuButton(GenMenuBar::Button button, ActionId acionId, QPixmap icon, bool enabled = true);
    void hideButtons(GenMenuBar::Buttons buttons);
    void showButtons(GenMenuBar::Buttons buttons);
    void enableButtons(GenMenuBar::Buttons buttons);
    void disableButtons(GenMenuBar::Buttons buttons);
    GenMenuBar::Buttons buttons();
    ActionId actionForButton(GenMenuBar::Button button);

protected slots:
    void handleHideButtonsFinished();
signals:
    void actionTriggered(const ActionId actionId);

private:
    Ui::GenMenuBar *ui;
    Buttons _buttons;
    QPixmap _icons[4];
    ActionId _actions[4];
    Buttons _enabledButtons;
    Buttons _transitionButtons;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GenMenuBar::Buttons)

#endif // GENMENUBAR_H
