#include "genpushbutton.h"
#include "genfeedbackutils.h"

GenPushButton::GenPushButton(QWidget *parent, ActionId actionId) :
    QPushButton(parent), _actionId(actionId)
{
    connect(this, SIGNAL(clicked()), SLOT(handleClicked()));
    connect(this, SIGNAL(pressed()), SLOT(handlePressed()));
    connect(this, SIGNAL(released()), SLOT(handleReleased()));
}

GenPushButton::~GenPushButton()
{

}

void GenPushButton::setAction(const ActionId actionId)
{
    _actionId = actionId;
}

ActionId GenPushButton::actionId() const
{
    return _actionId;
}

void GenPushButton::handleClicked()
{
    emit clicked(_actionId);
}

void GenPushButton::handlePressed()
{
    GenFeedbackUtils::instantFeedback(GenFeedbackUtils::ThemeBasicButton);
}

void GenPushButton::handleReleased()
{
    GenFeedbackUtils::instantFeedback(GenFeedbackUtils::ThemeBasicButton);
}
