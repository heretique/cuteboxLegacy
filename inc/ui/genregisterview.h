#ifndef GENREGISTERVIEW_H
#define GENREGISTERVIEW_H

#include "genbaseview.h"

namespace Ui {
    class GenRegisterView;
}

class GenRegisterView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenRegisterView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenRegisterView();

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
     void handleEditTextChanged(QString text);

signals:
     void registerAccount(QString firstName,
                          QString lastName,
                          QString email,
                          QString pass);

private:
    Ui::GenRegisterView *ui;
};

#endif // GENREGISTERVIEW_H
