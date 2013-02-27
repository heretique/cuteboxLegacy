#ifndef GENINDICATORSWIDGET_H
#define GENINDICATORSWIDGET_H

#include <QWidget>

namespace Ui {
    class GenIndicatorsWidget;
}

class GenIndicatorsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GenIndicatorsWidget(QWidget *parent = 0);
    ~GenIndicatorsWidget();
    void setSignalLevel(int level);
    void setBatteryLevel(int level);
    void setClock(QTime clock);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    Ui::GenIndicatorsWidget *ui;
};

#endif // GENINDICATORSWIDGET_H
