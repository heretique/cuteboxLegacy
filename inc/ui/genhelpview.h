#ifndef GENHELPVIEW_H
#define GENHELPVIEW_H

#include "genbaseview.h"
#include "genkineticscrollengine.h"


namespace Ui {
    class GenHelpView;
}

class QLabel;

class GenHelpView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenHelpView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenHelpView();
    void deleteUI();
    void createList();
    /*!
     * \brief Activates the view
     * \param data, a QVariant argument. Additional data.
     */
    void activate(const QVariant &data = QVariant());

    /*!
     * \brief Deactivates the view
     */
    void deactivate();

    void updateMenuBar(GenMenuBar &menuBar);

    bool handleAction(const ActionId actionId);

protected:
    void resizeEvent(QResizeEvent *e);
    void previousPage();
    void nextPage();
    void changePage(bool isForward);

protected slots:
    void handleFlicked(GenKineticScrollEngine::SwipeDirection direction);
    void changeAnimationFinished();

private:
    Ui::GenHelpView *ui;
    QStringList _images;
    int _currentImageIndex;
    QLabel *_newLabel;
    bool _animInProgress;
};

#endif // GENHELPVIEW_H
