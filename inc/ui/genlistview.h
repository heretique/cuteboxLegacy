#ifndef GENLISTVIEW_H
#define GENLISTVIEW_H

#include <QListView>

class GenListView : public QListView
{
public:
    GenListView(QWidget *parent = 0);
    ~GenListView();

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // GENLISTVIEW_H
