#include "genfeedbackutilssymimpl.h"

#ifdef Q_OS_SYMBIAN

#include <touchfeedback.h>
#include <touchlogicalfeedback.h>

GenFeedbackUtilsPrivate::GenFeedbackUtilsPrivate(GenFeedbackUtils *parent) :
    q_ptr(parent), _feedbackInstance(NULL)
{
    _feedbackInstance = MTouchFeedback::Instance();
    if (NULL == _feedbackInstance) {
        _feedbackInstance = MTouchFeedback::CreateInstanceL();
    }
    if (_feedbackInstance)
        _feedbackInstance->SetFeedbackEnabledForThisApp(ETrue);
}

GenFeedbackUtilsPrivate::~GenFeedbackUtilsPrivate()
{

}

void GenFeedbackUtilsPrivate::instantFeedback(
    GenFeedbackUtils::FeedBackType feedback)
{
    if (_feedbackInstance)
        _feedbackInstance->InstantFeedback(convertToSymbian(feedback));
}

TTouchLogicalFeedback GenFeedbackUtilsPrivate::convertToSymbian(
    GenFeedbackUtils::FeedBackType effect)
{

    TTouchLogicalFeedback themeFeedbackSymbian = ETouchFeedbackBasic;

    switch (effect) {
    case GenFeedbackUtils::ThemeBasic:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeSensitive:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case GenFeedbackUtils::ThemeBasicButton:
        themeFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case GenFeedbackUtils::ThemeSensitiveButton:
        themeFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case GenFeedbackUtils::ThemeBasicItem:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeSensitiveItem:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case GenFeedbackUtils::ThemeBounceEffect:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemePopupOpen:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemePopupClose:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeBasicSlider:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeSensitiveSlider:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case GenFeedbackUtils::ThemeStopFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeEditor:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeTextSelection:
        themeFeedbackSymbian = ETouchFeedbackTextSelection;
        break;
    case GenFeedbackUtils::ThemeBlankSelection:
        themeFeedbackSymbian = ETouchFeedbackBlankSelection;
        break;
    case GenFeedbackUtils::ThemeLineSelection:
        themeFeedbackSymbian = ETouchFeedbackLineSelection;
        break;
    case GenFeedbackUtils::ThemeEmptyLineSelection:
        themeFeedbackSymbian = ETouchFeedbackEmptyLineSelection;
        break;
    case GenFeedbackUtils::ThemeCheckBox:
        themeFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
    case GenFeedbackUtils::ThemeMultipleCheckBox:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeSensitiveKeypad:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
    case GenFeedbackUtils::ThemeBasicKeypad:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeMultiPointTouchActivate:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeRotateStep:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeItemDrop:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeItemMoveOver:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeItemPick:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemeItemScroll:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case GenFeedbackUtils::ThemePopUp:
        themeFeedbackSymbian = ETouchFeedbackPopUp;
        break;
    case GenFeedbackUtils::ThemeLongPress:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    default:
        break;
    }
    return themeFeedbackSymbian;
}

#endif
