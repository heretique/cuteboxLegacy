#ifndef GENFILEBROWSERVIEW_H
#define GENFILEBROWSERVIEW_H

#include "genbaseview.h"
#include "genkineticscrollengine.h"

namespace Ui {
    class GenFileBrowserView;
}

class GenListView;
class QAnimationGroup;
class QModelIndex;
class GenKineticScrollWidget;
class GenFileSystemFilterProxyModel;

class GenFileBrowserView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenFileBrowserView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenFileBrowserView();
public:
    enum State
    {
        StateListView,
        StateUploadSelection,
        StateFolderSelection,
        StateFileActionSelection,
        StateCopySelection,
        StateMoveSelection,
        StateDeleteSelection,
        StateSkipUpdate
    };

    /*!
     * \brief Activates the view
     * \param data, a QVariant argument. Additional data.
     */
    void activate(const QVariant &data = QVariant());
    void viewActivated();
    void aboutToDeactivate();

    /*!
     * \brief Deactivates the view
     */
    void deactivate();
    void changeFolder(const QModelIndex &index, bool isForward = true);
    void updateMenuBar(GenMenuBar &menuBar);
    bool handleAction(const ActionId actionId);
    void setState(State state);

protected:
    void resizeEvent(QResizeEvent *e);
    QWidget *primaryWidget();
    void createNewList();

    void handleItemSelected(const QModelIndex &index);
    void listItem(const QModelIndex &index);
    void getSelection();

protected slots:
    void changeAnimationFinished();
    void handleItemClicked(const QModelIndex &index);
    void handleBack();
    void handleFlicked(GenKineticScrollEngine::SwipeDirection direction);

private:
    Ui::GenFileBrowserView *ui;
    GenKineticScrollWidget *_newKineticArea;
    GenListView *_newList;
    bool _animInProgress;
    int _selectionCount;
    State _state;
    GenFileSystemFilterProxyModel *_filterModel;
};


#endif // GENFILEBROWSERVIEW_H
