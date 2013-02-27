#ifndef _GENFEEDBACKUTILS_H_
#define _GENFEEDBACKUTILS_H_

#include <QObject>

class GenFeedbackUtilsPrivate;

class GenFeedbackUtils: public QObject
{
Q_OBJECT
Q_ENUMS(FeedBackType)
public:
    virtual ~GenFeedbackUtils();
    enum FeedBackType
    {
        ThemeBasic,
        ThemeSensitive,
        ThemeBasicButton,
        ThemeSensitiveButton,
        ThemeBasicKeypad,
        ThemeSensitiveKeypad,
        ThemeBasicSlider,
        ThemeSensitiveSlider,
        ThemeBasicItem,
        ThemeSensitiveItem,
        ThemeItemScroll,
        ThemeItemPick,
        ThemeItemDrop,
        ThemeItemMoveOver,
        ThemeBounceEffect,
        ThemeCheckBox,
        ThemeMultipleCheckBox,
        ThemeEditor,
        ThemeTextSelection,
        ThemeBlankSelection,
        ThemeLineSelection,
        ThemeEmptyLineSelection,
        ThemePopUp,
        ThemePopupOpen,
        ThemePopupClose,
        ThemeFlick,
        ThemeStopFlick,
        ThemeMultiPointTouchActivate,
        ThemeRotateStep,
        ThemeLongPress,
        ThemePositiveTacticon,
        ThemeNeutralTacticon,
        ThemeNegativeTacticon,
        NumberOfThemeEffects,
        ThemeUser = 65535
    };

private:
    GenFeedbackUtils();

    static GenFeedbackUtils &instance();
    
    
public slots:
    static void instantFeedback(GenFeedbackUtils::FeedBackType feedback);

protected:
    GenFeedbackUtilsPrivate * const d_ptr;
private:
    Q_DECLARE_PRIVATE(GenFeedbackUtils)
};

#endif // PROFILEUTILS_H
