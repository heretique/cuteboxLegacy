#ifndef GENLOGINVIEW_H
#define GENLOGINVIEW_H

#include "genbaseview.h"

namespace Ui
{
class GenLoginView;
}

class GenLoginView: public GenBaseView
{
Q_OBJECT

public:
    explicit GenLoginView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenLoginView();

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
    void loginClicked();
    void handleEditTextChanged(QString text);

signals:
    void startOauthTokenRequest(QString userName, QString userPass);
    
private:
    Ui::GenLoginView *ui;
};

#endif // GENLOGINVIEW_H
