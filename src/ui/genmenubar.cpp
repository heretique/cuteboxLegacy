#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "genmenubar.h"
#include "ui_genmenubar.h"

GenMenuBar::GenMenuBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenMenuBar),
    _buttons(0),
    _enabledButtons(0),
    _transitionButtons(0)
{
    ui->setupUi(this);
    connect(ui->action1, SIGNAL(clicked(const ActionId)), this, SIGNAL(actionTriggered(const ActionId)));
    connect(ui->action2, SIGNAL(clicked(const ActionId)), this, SIGNAL(actionTriggered(const ActionId)));
    connect(ui->action3, SIGNAL(clicked(const ActionId)), this, SIGNAL(actionTriggered(const ActionId)));
    connect(ui->action4, SIGNAL(clicked(const ActionId)), this, SIGNAL(actionTriggered(const ActionId)));
}

GenMenuBar::~GenMenuBar()
{
    delete ui;
}

void GenMenuBar::setMenuActions(const ActionId action1,
                                const ActionId action2,
                                const ActionId action3,
                                const ActionId action4)
{
    _actions[0] = action1;
    _actions[1] = action2;
    _actions[2] = action3;
    _actions[3] = action4;
}

void GenMenuBar::setMenuIcons(QPixmap icon1,
                              QPixmap icon2,
                              QPixmap icon3,
                              QPixmap icon4)
{
    _icons[0] = icon1;
    _icons[1] = icon2;
    _icons[2] = icon3;
    _icons[3] = icon4;
}

void GenMenuBar::setMenuButtons(GenMenuBar::Buttons buttons, GenMenuBar::Buttons enabledButtons)
{
    _buttons = buttons;
    _enabledButtons = enabledButtons;
    hideButtons(GenMenuAllButtons);
}

void GenMenuBar::setMenuButton(GenMenuBar::Button button, ActionId actionId, QPixmap icon, bool enabled)
{
    _buttons |= button;
    if (button == GenMenuButton1)
    {
        _actions[0] = actionId;
        _icons[0] = icon;
    }
    else if (button == GenMenuButton2)
    {
        _actions[1] = actionId;
        _icons[1] = icon;
    }
    else if (button == GenMenuButton3)
    {
        _actions[2] = actionId;
        _icons[2] = icon;
    }
    else if (button == GenMenuButton4)
    {
        _actions[3] = actionId;
        _icons[3] = icon;
    }
    if (enabled)
        _enabledButtons |= button;
    else
        _enabledButtons &=(~button);
    hideButtons(button);
}

void GenMenuBar::handleHideButtonsFinished()
{
    if (_transitionButtons.testFlag(GenMenuButton1))
    {
        ui->action1->setAction(_actions[0]);
        ui->action1->setIcon(QIcon(_icons[0]));
    }
    if (_transitionButtons.testFlag(GenMenuButton2))
    {
        ui->action2->setAction(_actions[1]);
        ui->action2->setIcon(QIcon(_icons[1]));
    }
    if (_transitionButtons.testFlag(GenMenuButton3))
    {
        ui->action3->setAction(_actions[2]);
        ui->action3->setIcon(QIcon(_icons[2]));
    }
    if (_transitionButtons.testFlag(GenMenuButton4))
    {
        ui->action4->setAction(_actions[3]);
        ui->action4->setIcon(QIcon(_icons[3]));
    }
    showButtons(_transitionButtons);
}

void GenMenuBar::hideButtons(GenMenuBar::Buttons buttons)
{
    _transitionButtons = buttons;
    QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(this);

    if (buttons.testFlag(GenMenuButton1) && ui->action1->iconSize() != QSize(0,0))
    {
        QPropertyAnimation *action1Ani = new QPropertyAnimation(ui->action1, "iconSize", this);
        action1Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action1Ani->setStartValue(QSize(48, 48));
        action1Ani->setEndValue(QSize(0, 0));
        action1Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action1Ani);
        ui->action1->setEnabled(false);
    }

    if (buttons.testFlag(GenMenuButton2) && ui->action2->iconSize() != QSize(0,0))
    {
        QPropertyAnimation *action2Ani = new QPropertyAnimation(ui->action2, "iconSize", this);
        action2Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action2Ani->setStartValue(QSize(48, 48));
        action2Ani->setEndValue(QSize(0, 0));
        action2Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action2Ani);
        ui->action2->setEnabled(false);
    }

    if (buttons.testFlag(GenMenuButton3) && ui->action3->iconSize() != QSize(0,0))
    {
        QPropertyAnimation *action3Ani = new QPropertyAnimation(ui->action3, "iconSize", this);
        action3Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action3Ani->setStartValue(QSize(48, 48));
        action3Ani->setEndValue(QSize(0, 0));
        action3Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action3Ani);
        ui->action3->setEnabled(false);
    }

    if (buttons.testFlag(GenMenuButton4)  && ui->action4->iconSize() != QSize(0,0))
    {
        QPropertyAnimation *action4Ani = new QPropertyAnimation(ui->action4, "iconSize", this);
        action4Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action4Ani->setStartValue(QSize(48, 48));
        action4Ani->setEndValue(QSize(0, 0));
        action4Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action4Ani);
        ui->action4->setEnabled(false);
    }
    connect(aniGroup, SIGNAL(finished()), this, SLOT(handleHideButtonsFinished()));
    aniGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
}

void GenMenuBar::showButtons(GenMenuBar::Buttons buttons)
{
    QParallelAnimationGroup* aniGroup = new QParallelAnimationGroup(this);

    if (buttons.testFlag(GenMenuButton1))
    {
        QPropertyAnimation *action1Ani = new QPropertyAnimation(ui->action1, "iconSize", this);
        action1Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action1Ani->setStartValue(QSize(0, 0));
        action1Ani->setEndValue(QSize(48, 48));
        action1Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action1Ani);
        if(_enabledButtons.testFlag(GenMenuButton1))
            ui->action1->setEnabled(true);
    }

    if (buttons.testFlag(GenMenuButton2))
    {
        QPropertyAnimation *action2Ani = new QPropertyAnimation(ui->action2, "iconSize", this);
        action2Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action2Ani->setStartValue(QSize(0, 0));
        action2Ani->setEndValue(QSize(48, 48));
        action2Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action2Ani);
        if(_enabledButtons.testFlag(GenMenuButton2))
            ui->action2->setEnabled(true);
    }

    if (buttons.testFlag(GenMenuButton3))
    {
        QPropertyAnimation *action3Ani = new QPropertyAnimation(ui->action3, "iconSize", this);
        action3Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action3Ani->setStartValue(QSize(0, 0));
        action3Ani->setEndValue(QSize(48, 48));
        action3Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action3Ani);
        if(_enabledButtons.testFlag(GenMenuButton3))
            ui->action3->setEnabled(true);
    }

    if (buttons.testFlag(GenMenuButton4))
    {
        QPropertyAnimation *action4Ani = new QPropertyAnimation(ui->action4, "iconSize", this);
        action4Ani->setDuration(MENU_CHANGE_ANIM_DURATION);
        action4Ani->setStartValue(QSize(0, 0));
        action4Ani->setEndValue(QSize(48, 48));
        action4Ani->setEasingCurve(QEasingCurve::InOutCubic);
        aniGroup->addAnimation(action4Ani);
        if(_enabledButtons.testFlag(GenMenuButton4))
            ui->action4->setEnabled(true);
    }

    aniGroup->start(QParallelAnimationGroup::DeleteWhenStopped);
}

void GenMenuBar::enableButtons(GenMenuBar::Buttons buttons)
{
    if (buttons.testFlag(GenMenuButton1))
        ui->action1->setEnabled(true);

    if (buttons.testFlag(GenMenuButton2))
        ui->action2->setEnabled(true);

    if (buttons.testFlag(GenMenuButton3))
        ui->action3->setEnabled(true);

    if (buttons.testFlag(GenMenuButton4))
        ui->action4->setEnabled(true);
}

void GenMenuBar::disableButtons(GenMenuBar::Buttons buttons)
{
    if (buttons.testFlag(GenMenuButton1))
        ui->action1->setEnabled(false);

    if (buttons.testFlag(GenMenuButton2))
        ui->action2->setEnabled(false);

    if (buttons.testFlag(GenMenuButton3))
        ui->action3->setEnabled(false);

    if (buttons.testFlag(GenMenuButton4))
        ui->action4->setEnabled(false);
}

GenMenuBar::Buttons GenMenuBar::buttons()
{
    return _buttons;
}

ActionId GenMenuBar::actionForButton(GenMenuBar::Button button)
{
    if (_buttons.testFlag(button))
    {
        if (button == GenMenuButton1)
            return _actions[0];
        if (button == GenMenuButton2)
            return _actions[1];
        if (button == GenMenuButton3)
            return _actions[2];
        if (button == GenMenuButton4)
            return _actions[3];
    }
    return ActionNONE;
}
