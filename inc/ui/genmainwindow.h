#ifndef GENMAINWINDOW_H
#define GENMAINWINDOW_H

#include <QWidget>
#include <QAbstractAnimation>
#include <QVariant>

#include "genenums.h"
#include "gentypes.h"

namespace Ui {
    class GenMainWindow;
}

class QAnimationGroup;
class GenBaseView;
class GenQueryPopup;
class GenWaitPopup;
class GenFileProgress;
class GenStatusWidget;
class GenMenuBar;
class GenPopupMenu;
class GenFilenamePopup;
class GenAppModel;

class GenMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GenMainWindow(GenAppModel &model, QWidget *parent = 0);
    ~GenMainWindow();

    /*!
     * \brief Adds a top level widget to the stacked widget
     * \param widget, a QWidget* argument
     */
    void activateView(QString className, const QVariant &data = QVariant(),
        bool useAni = true, GenViewAnimType animType = GenFromRight);

    void setViewData(QString className, const QVariant &data = QVariant());

    void activatePreviousView();
    /*!
     * \brief Removes a widget from the stacked widget
     * \param widget, a QWidget* argument
     */
    void removeView(QString className);

    /*!
     * \brief Returns a pointer to the top level widget
     * \return QWidget*
     */
    GenBaseView *currentView();

    void addView(GenBaseView *view);

    void setDefaultView(GenBaseView *view);

    GenBaseView *view(QString className);

    GenStatusWidget *statusBar();

    GenFileProgress *fileProgressPopup();
    GenQueryPopup *queryPopup();
    GenMenuBar *menuBar();
    void setQueryText(const QString& text);
    void setQueryAction(const ActionId action);
    bool popupOpened();
    void useImageBackground(bool useImage);
    void showNotificationInfo(QString info, bool autoclose = true);
    /**
     * Creates and returns the view animation
     * @param currentVisibleView, view that is noe visible
     * @param nextVisibleView, view that will become visible after animation
     * @param isForward, animation direction, true when advancing forward in app
     * @return QAnimationGroup, created animation
     */
    QAnimationGroup *createViewAnimation(QWidget& currentVisibleView,
                                         QWidget& nextVisibleView,
                                         GenViewAnimType animType,
                                         bool disableDuringAnim = true);
protected:
    
    void resizeEvent (QResizeEvent* event);

private:

    QAnimationGroup *createPopupShowAnimation(QWidget &popup);
    QAnimationGroup *createPopupHideAnimation(QWidget &popup);

public slots:
    void showWaitingIndicator(QString text, bool lockInput = true);
    void hideWaitingIndicator();
    void showPopupMenu(QString caption, QList<QPair<ActionId, QString> > actions);
    void showQueryPopup(const QString& text);
    void showFileProgressPopup(const QString& filename);
    void hideFileProgressPopup();
    void showFilenamePopup();
    void fileProgress(qint64 bytes, qint64 bytesTotal);
    void totalProgress(int current, int total);

protected slots:
    void handleActionTriggered(const ActionId actionId);
    void activateAnimationFinished();
    void handlePopupClosed(const ActionId actionId);
    void handleFilenamePopupClosed(const ActionId actionId);
    void handleFileActionCanceled();
    void popupShowAnimationFinished();
    void popupHideAnimationFinished();
    void activateAnimationStateChanged(QAbstractAnimation::State newState,
        QAbstractAnimation::State oldState);
    void handleSettingsClicked();
    void handleButtonTouch();
    void hidePopupMenu();
    void hideQueryPopup();
    void hideFilenamePopup();
    void handlePopupMenuCloseSingleShot();
signals:

    void animationFinished();
    void actionTriggered(const ActionId actionId);

private:
    Ui::GenMainWindow *ui; /*!< Generated ui interface class */
    GenAppModel &_appModel;
    GenBaseView *_currentView;
    GenBaseView *_newView;
    bool _animInProgress;
    bool _popupAnimInProgress;
    GenQueryPopup *_queryPopup;
    GenFileProgress *_fileProgressPopup;
    GenPopupMenu *_popupMenu;
    GenFilenamePopup *_fileNamePopup;
    bool _isPopupOpen;
    bool _isProgressOpen;
    bool _isPopupMenuOpen;
    bool _isFilenamePopupOpen;
    ActionId _popupAction;
};

#endif // GENMAINWINDOW_H
