#ifndef GENWAITPOPUP_H
#define GENWAITPOPUP_H

#include <QWidget>

namespace Ui {
    class GenWaitPopup;
}

class GenWaitPopup : public QWidget
{
    Q_OBJECT

public:
    explicit GenWaitPopup(QWidget *parent = 0);
    ~GenWaitPopup();
    void start();
    void stop();
    void setText(QString text);
private:
    Ui::GenWaitPopup *ui;
};

#endif // GENWAITPOPUP_H
