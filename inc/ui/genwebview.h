#ifndef GENWEBVIEW_H
#define GENWEBVIEW_H

#include "genbaseview.h"
#include <QUrl>

namespace Ui {
class GenWebView;
}

class GenWebView : public GenBaseView
{
    Q_OBJECT
    
public:
    explicit GenWebView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent = 0);
    ~GenWebView();

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

    void setViewData(const QVariant &data);

protected:
    void resizeEvent(QResizeEvent *e);

protected slots:
    void handleUrlChanged(QUrl url);

signals:
    void authorized(QString uid, QString authToken);
    void notAuthorized();

private:
    Ui::GenWebView *ui;
};

#endif // GENWEBVIEW_H
