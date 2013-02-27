#ifndef GENPHOTOVIEW_H
#define GENPHOTOVIEW_H

#include "genbaseview.h"

namespace Ui {
    class GenPhotoView;
}

class GenWSEngine;

class GenPhotoView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenPhotoView(GenWSEngine *wsEngine,
                          GenAppModel *appModel,
                          GenMainWindow* mainWindow,
                          QWidget *parent = 0);
    ~GenPhotoView();
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

private:
    Ui::GenPhotoView *ui;
};

#endif // GENPHOTOVIEW_H
