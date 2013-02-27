#ifndef GENSTATUSWIDGET_H
#define GENSTATUSWIDGET_H

#include <QWidget>
#include "genpushbutton.h"

namespace Ui {
    class GenStatusWidget;
}


class GenIndicatorsWidget;

class GenStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GenStatusWidget(QWidget *parent = 0);
    ~GenStatusWidget();

    void setText(const QString& text);
    const GenPushButton *backButton() const;
    const GenPushButton *indicatorButton() const;
    void setIndicatorIcon(QPixmap icon);
    GenIndicatorsWidget *indicatorsWidget();
    void showHomeButton(bool show);
    void showExitButton(bool show);

public slots:
    void showWaitingIndicator(QString text);
    void hideWaitingIndicator();

signals:
    void actionTriggered(const ActionId actionId);

private:
    Ui::GenStatusWidget *ui;
    QString _text;
    QString _waitingText;
    bool _waiting;
    bool _showHomeIndicator;
};

#endif // GENSTATUSWIDGET_H
