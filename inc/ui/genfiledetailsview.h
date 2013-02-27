#ifndef GENFILEDETAILSVIEW_H
#define GENFILEDETAILSVIEW_H

#include <QNetworkReply>
#ifndef Q_OS_WIN32
#include <QMessageService>

QTM_USE_NAMESPACE
#endif

#include "genbaseview.h"
#include "genkineticscrollengine.h"

namespace Ui {
    class GenFileDetailsView;
}

class GenWSEngine;
class GenFileDownloader;
class GenOAuthRequest;


class GenFileDetailsView : public GenBaseView
{
    Q_OBJECT

public:
    explicit GenFileDetailsView(GenWSEngine *wsEngine,
                                GenAppModel *appModel,
                                GenMainWindow* mainWindow,
                                QWidget *parent = 0);
    ~GenFileDetailsView();

public:

    /*!
     * \brief Activates the view
     * \param data, a QVariant argument. Additional data.
     */
    void activate(const QVariant &data = QVariant());

    void viewActivated();

    /*!
     * \brief Deactivates the view
     */
    void deactivate();

    void updateMenuBar(GenMenuBar &menuBar);

    bool handleAction(const ActionId actionId);

protected:
    enum State {
        StateNormal = 0,
        StateChooseShareOptions
    };

    void resizeEvent(QResizeEvent *e);
    void deleteUi();
    QString localFilename(QString dropboxPath);
    void aboutToDeactivate();
    void setOpenButton();
    void showSendLinkMenu();
    void showEmailAccountSelectionMenu();
    QString createPublicLink();
    void copyLinkToClipboard();
    void sendLinkBySMS();
    void sendLinkByEmail();
    bool isImage(QString filename);
    void createThumbnail(QString filename, int size);
    void setShareMenu(GenMenuBar &menuBar);


#ifndef Q_OS_WIN32
    QMessage composeMessage();
#endif

protected slots:
    void handleFlicked(GenKineticScrollEngine::SwipeDirection direction);
    void handleDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void handleDownloadFinished(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply);
    void handleDownloadStarted(const QString& filename);
    void startDownload();
    void startCreateThumbnail();
    void showQRCode();

private:
    Ui::GenFileDetailsView *ui;
    GenWSEngine *_wsEngine;
    GenOAuthRequest *_oauthRequest;
    GenFileDownloader *_fileDownloader;
    bool _localUpToDate;
    State _state;
    QString _localFilename;
    QPixmap _thumbnail;
};

#endif // GENFILEDETAILSVIEW_H
