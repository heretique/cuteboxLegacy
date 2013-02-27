#ifndef GENFILENAMEPOPUP_H
#define GENFILENAMEPOPUP_H

#include "geninterfaces.h"

namespace Ui {
    class GenFilenamePopup;
}

class GenMenuBar;
class GenAppModel;

class GenFilenamePopup : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenFilenamePopup(GenMenuBar &menuBar, GenAppModel &appModel, QWidget *parent = 0);
    ~GenFilenamePopup();
    void updateMenuBar(GenMenuBar &menuBar);

public slots:
    bool handleAction(const ActionId actionId);

signals:
    void popupClosed(const ActionId actionId);

private slots:
    void on_lineEdit_textChanged(QString text);

private:
    Ui::GenFilenamePopup *ui;
    GenMenuBar &_menuBar;
    GenAppModel &_appModel;
    bool _nameEmpty;
};

#endif // GENFILENAMEPOPUP_H
