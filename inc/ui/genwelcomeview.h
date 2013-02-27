#ifndef GENWELCOMEVIEW_H
#define GENWELCOMEVIEW_H

#include "genbaseview.h"

namespace Ui
{
class GenWelcomeView;
}

class GenWelcomeView: public GenBaseView
{
Q_OBJECT

public:
    explicit GenWelcomeView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenWelcomeView();

public:

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

protected slots:
    
    void oldUserButtonClicked();
    void newUserButtonClicked();
    void handleDropboxCliked();

private:
    Ui::GenWelcomeView *ui;
};

#endif // GENWELCOMEVIEW_H
