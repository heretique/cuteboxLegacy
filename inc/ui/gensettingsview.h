#ifndef GENSETTINGSVIEW_H
#define GENSETTINGSVIEW_H

#include "genbaseview.h"

namespace Ui {
    class GenSettingsView;
}

class GenAppModel;

class GenSettingsView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenSettingsView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenSettingsView();

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

    void setOKButton();

public slots:
    void handleModelChanged(GenAppModel& appModel);

protected slots:
    bool handleAction(const ActionId actionId);
    void handleCheckStateChanged(int state);

signals:
    void getAccountInfo();

protected:
    void resizeEvent(QResizeEvent *e);
    void retrieveSettings();

private:
    qreal getUsedSpacePercent(qint64 usedSpace, qint64 sharedSpace, qint64 totalSpace);

private:
    Ui::GenSettingsView *ui;
    QString _previousView;
};

#endif // GENSETTINGSVIEW_H
