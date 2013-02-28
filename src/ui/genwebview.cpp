#include <QResizeEvent>

#include "genwebview.h"
#include "ui_genwebview.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genmenubar.h"
#include "genresourcereader.h"
#include "gendropboxrequests.h"
#include "genutils.h"
#include "genlog.h"

GenWebView::GenWebView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent), ui(NULL)
{

}

GenWebView::~GenWebView()
{
    delete ui;
}

void GenWebView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenWebView;
    ui->setupUi(this);
    connect(ui->webView,
            SIGNAL(urlChanged(QUrl)),
            SLOT(handleUrlChanged(QUrl)));
}



void GenWebView::deactivate()
{
    delete ui->webView;
    ui->webView = NULL;
    delete ui;
    ui = NULL;
}

void GenWebView::updateMenuBar(GenMenuBar &menuBar)
{
    setCancelMenuButton(menuBar);
}

bool GenWebView::handleAction(const ActionId actionId)
{
    if (ActionCANCEL == actionId)
    {
        activateView("GenWelcomeView", QVariant(), true, GenFromBottom);
        return true;
    }
//    if (ActionOK == actionId)
//    {
//        loginClicked();
//    }
    return false;
}

void GenWebView::setViewData(const QVariant &data)
{
    QString url = data.toString();
    ui->webView->setUrl(url);
}

void GenWebView::resizeEvent(QResizeEvent *e)
{
    if (ui)
        ui->webView->resize(e->size());
    QWidget::resizeEvent(e);
}

void GenWebView::handleUrlChanged(QUrl url)
{
    QLOG_DEBUG("[GenApplicationManager::handleAuthUrlChanged]- url = " << url);
    QLOG_DEBUG("[GenApplicationManager::handleAuthUrlChanged]- url host = " << url.host());

    _mainWindow->hideWaitingIndicator();

    if (url.host().contains(REDIRECT_HOST, Qt::CaseInsensitive)) {
        QString notApproved = url.queryItemValue("not_approved");
        if (notApproved.compare("true") == 0)
            emit notAuthorized();
        else
        {
            QString uid = url.queryItemValue("uid");
            QString token = url.queryItemValue("oauth_token");
            emit authorized(uid, token);
        }
    }
}
