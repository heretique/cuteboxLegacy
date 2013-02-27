#ifndef UIGENMAINVIEW_H
#define UIGENMAINVIEW_H

#include <QThread>
#include "genbaseview.h"
#include "genkineticscrollengine.h"
#include "genmediagallery.h"


namespace Ui {
    class GenMainView;
}

class GenListView;
class QAnimationGroup;
class GenFileInfoModel;
class QModelIndex;
class GenKineticScrollWidget;
class GenWSEngine;
class GenFileInfo;
class GenThumbnailManager;

class GenMainView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenMainView(GenWSEngine *wsEngine,
                         GenAppModel *appModel,
                         GenMainWindow* mainWindow,
                         QWidget *parent = 0);
    ~GenMainView();
public:
    enum State
    {
        StateListView,
        StateDownloadSelection,
        StateUploadSelection,
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
    /*!
     * \brief Deactivates the view
     */
    void deactivate();
    
    void changeFileInfoModel(GenFileInfoModel *newModel, bool isForward = true);

    void setState(State state);

    void updateMenuBar(GenMenuBar &menuBar);

    bool handleAction(const ActionId actionId);

protected:
    void resizeEvent(QResizeEvent *e);
    void listFiles(const QString &path, bool isForward);
    void createNewList();

    void handleItemSelected(const QModelIndex &index);
    void listItem(const QModelIndex &index);
    void setOKCancelMenuButtons();
    void setFileActionsButtons();
    void getSelection();
    void connectSlots();
    void disconnectSlots();
    void disconnectModelSlots();
    void aboutToDeactivate();
    void connectModelSlots(GenFileInfoModel *model);
    void uploadMediaFiles(GenMediaGallery::MediaFileTypes mediaType);

    void handleDownloadFiles();
    void handleCopyFiles();
    void handleMoveFiles();
    void handleDeleteFiles();
    void handleUploadFiles();
    void handleCreateFolder();

protected slots:
    void changeAnimationFinished();
    void handleItemClicked(const QModelIndex &index);
    void handleBack();
    void handleFlicked(GenKineticScrollEngine::SwipeDirection direction);
    void handleFileInfoReceived(GenFileInfo *parent, QList<GenFileInfo*> content);
    void handleMetaDataNotModified();
    void refreshView();
    void handleUploadFinished();
    void handleFolderCreated(GenFileInfo *fileInfo);

private:
    Ui::GenMainView *ui;
    GenWSEngine *_wsEngine;
    GenKineticScrollWidget *_newKinetiArea;
    GenListView *_newList;
    GenFileInfoModel *_newModel;
    bool _animInProgress;
    State _state;
    int _selectionCount;
    bool _isForward;
    QString _uploadPath;
    GenThumbnailManager *_thumbnailManager;
};

#endif // UIGENMAINVIEW_H
