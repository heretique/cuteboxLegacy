#ifndef GENDOWNLOADSVIEW_H
#define GENDOWNLOADSVIEW_H

#include <QWidget>

namespace Ui {
    class GenDownloadsView;
}

class GenDownloadsView : public QWidget
{
    Q_OBJECT

public:
    explicit GenDownloadsView(QWidget *parent = 0);
    ~GenDownloadsView();

private:
    Ui::GenDownloadsView *ui;
};

#endif // GENDOWNLOADSVIEW_H
