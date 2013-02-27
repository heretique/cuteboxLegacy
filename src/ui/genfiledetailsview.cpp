#include <QResizeEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QImageReader>

#if defined(Q_OS_SYMBIAN)
#include <eikedwin.h>
#include <baclipb.h>
#include <s32ucmp.h>
#include <txtclipboard.h>
#endif

#include "genfiledetailsview.h"
#include "ui_genfiledetailsview.h"
#include "genconstants.h"
#include "genfileinfo.h"
#include "genfileinfomodel.h"
#include "genappmodel.h"
#include "genliterals.h"
#include "genresourcereader.h"
#include "genmenubar.h"
#include "genutils.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genmainview.h"
#include "genwsengine.h"
#include "genfiledownloader.h"
#include "genoauthrequest.h"
#include "genqrencoder.h"
#include "genlog.h"

GenFileDetailsView::GenFileDetailsView(GenWSEngine *wsEngine,
                                       GenAppModel *appModel,
                                       GenMainWindow* mainWindow,
                                       QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent),
    ui(NULL),
    _wsEngine(wsEngine),
    _oauthRequest(new GenOAuthRequest(this)),
    _fileDownloader(NULL),
    _localUpToDate(false),
    _state(StateNormal)
{

}

GenFileDetailsView::~GenFileDetailsView()
{
    delete _fileDownloader;
    delete ui;
}

void GenFileDetailsView::activate(const QVariant &data)
{
    if (ui) {
        delete ui;
        ui = NULL;
    }
    ui = new Ui::GenFileDetailsView;
    ui->setupUi(this);
    ui->itemProgressBar->hide();
    ui->itemProgressBar->reset();
    ui->kineticArea->enableKineticScrollingFor(ui->scrollArea, Qt::Vertical);
    connect(ui->kineticArea->kineticEngine(), SIGNAL(flicked(GenKineticScrollEngine::SwipeDirection)),
            SLOT(handleFlicked(GenKineticScrollEngine::SwipeDirection)));
    const GenFileInfo *fileInfo = _appModel->currentFileInfo();
    if (fileInfo)
    {
        QString shortFilename = fileInfo->_path.mid(fileInfo->_path.lastIndexOf("/") + 1);
        ui->filename->setText(shortFilename);
        ui->modified->setText(Duration(fileInfo->_modifiedTime));
        ui->size->setText(FileSize(fileInfo->_size));
        QString extension = shortFilename.mid(shortFilename.lastIndexOf(".") + 1);
        _thumbnail = GenResourceReader::thumbnailForFileExtension(extension);
        ui->thumbnail->setPixmap(_thumbnail);

        _fileDownloader = new GenFileDownloader(this);
        _fileDownloader->handleModelChanged(*_appModel);
        connect(_fileDownloader,
                SIGNAL(downloadStarted(QString)),
                SLOT(handleDownloadStarted(QString)));
        connect(_fileDownloader,
                SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(handleDownloadProgress(qint64,qint64)));
        connect(_fileDownloader,
                SIGNAL(downloadFinished(QNetworkReply::NetworkError,int,QByteArray)),
                SLOT(handleDownloadFinished(QNetworkReply::NetworkError,int,QByteArray)));

        _localFilename = localFilename(fileInfo->_path);
        if (!_localFilename.isEmpty())
        {
            QFileInfo lfi(_localFilename);
            QDateTime lmt = lfi.lastModified().toUTC();

            //!!!MEGA HACK
            // there is a strange bug, the last modified time saved to symbian
            // and the last time retrieved afterward are different by 1 second
            // but only sometimes
            if (lmt == fileInfo->_modifiedTime || lmt.addSecs(1) == fileInfo->_modifiedTime)
                _localUpToDate = true;
            else
                _localUpToDate = false;
        }
        else
        {
            _localUpToDate = false;
        }
        _mainWindow->statusBar()->showHomeButton(false);
        _mainWindow->statusBar()->setText(fileInfo->_path);
    }
}

void GenFileDetailsView::viewActivated()
{
    if (!_localUpToDate && _appModel->autoDownload())
    {
        QTimer::singleShot(VIEW_ANIMATION_DURATION, this, SLOT(startDownload()));
    }
    else
    {
        QTimer::singleShot(VIEW_ANIMATION_DURATION, this, SLOT(startCreateThumbnail()));
    }
}

void GenFileDetailsView::deactivate()
{
    QLOG_DEBUG("GenFileDetailsView::deactivate()");
    deleteUi();
    _state = StateNormal;
}

void GenFileDetailsView::updateMenuBar(GenMenuBar &menuBar)
{
    if (StateNormal == _state)
    {
        ActionId openAction = ActionNONE;
        QPixmap openIcon = QPixmap();
        ActionId linkAction = ActionNONE;
        QPixmap linkIcon = QPixmap();
        ActionId clipBoardAction = ActionNONE;
        QPixmap clipBoardIcon = QPixmap();
        GenMenuBar::Buttons buttons = GenMenuBar::GenMenuButton4;
        if (_localUpToDate)
        {
            openAction = ActionOPENFILE;
            openIcon = GenResourceReader::pixmapFromResource("open_icon.png");
            buttons |= GenMenuBar::GenMenuButton1;
        }
        else if (!_appModel->autoDownload())
        {
            openAction = ActionDOWNLOADFILES;
            openIcon = GenResourceReader::pixmapFromResource("download_icon.png");
            buttons |= GenMenuBar::GenMenuButton1;
        }
        if (_appModel->currentDropboxPath().startsWith("dropbox/Public"))
        {
            linkAction = ActionSHARE;
            linkIcon = GenResourceReader::pixmapFromResource("share_icon.png");
            clipBoardAction = ActionCOPYLINKTOCLIPBOARD;
            clipBoardIcon = GenResourceReader::pixmapFromResource("clipboard_icon.png");
            buttons |= GenMenuBar::GenMenuButton2 | GenMenuBar::GenMenuButton3;
        }

        menuBar.setMenuActions(openAction,
                               linkAction,
                               clipBoardAction,
                               ActionSETTINGS);
        menuBar.setMenuIcons(openIcon,
                             linkIcon,
                             clipBoardIcon,
                             GenResourceReader::pixmapFromResource("settings_icon.png"));
        menuBar.setMenuButtons(buttons, buttons);
    }
    else if (StateChooseShareOptions == _state)
        setShareMenu(menuBar);
}

void GenFileDetailsView::setShareMenu(GenMenuBar &menuBar)
{
    GenMenuBar::Buttons buttons = GenMenuBar::GenMenuButton1 | GenMenuBar::GenMenuButton2 | GenMenuBar::GenMenuButton4;

    menuBar.setMenuActions(ActionSENDLINK,
                           ActionQRCODE,
                           ActionNONE,
                           ActionCANCELSHARE);
    menuBar.setMenuIcons(GenResourceReader::pixmapFromResource("email_link_icon.png"),
                         GenResourceReader::pixmapFromResource("qrcode_icon.png"),
                         QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(buttons, buttons);
}

bool GenFileDetailsView::handleAction(const ActionId actionId)
{
    switch(actionId)
    {
    case ActionBACK:
        activateView("GenMainView", (int)GenMainView::StateSkipUpdate, true, GenFromLeft);
        return true;
    case ActionOPENFILE:
        if (!_localFilename.isEmpty())
            QDesktopServices::openUrl(QUrl("file:///" + _localFilename));
        return true;
    case ActionSETTINGS:
        _mainWindow->activateView("GenSettingsView",
                                  "GenFileDetailsView",
                                  true,
                                  GenFromTop);
        return true;
    case ActionDOWNLOADFILES:
        startDownload();
        break;
    case ActionSENDLINK:
        showSendLinkMenu();
        break;
    case ActionCOPYLINKTOCLIPBOARD:
        copyLinkToClipboard();
        break;
    case ActionSENDLINKBYSMS:
        sendLinkBySMS();
        _state = StateNormal;
        updateMenuBar(*(_mainWindow->menuBar()));
        break;
    case ActionSENDLINKBYEMAIL:
        sendLinkByEmail();
        _state = StateNormal;
        updateMenuBar(*(_mainWindow->menuBar()));
        break;
    case ActionSHARE:
        _state = StateChooseShareOptions;
        setShareMenu(*(_mainWindow->menuBar()));
        break;
    case ActionQRCODE:
        showQRCode();
        break;
    case ActionCANCELSHARE:
        _state = StateNormal;
        ui->thumbnail->setPixmap(_thumbnail);
        updateMenuBar(*(_mainWindow->menuBar()));
        break;
    default:
        break;
    }
    return false;
}

void GenFileDetailsView::resizeEvent(QResizeEvent *e)
{
    if (ui)
    {
        ui->kineticArea->resize(e->size());
        ui->scrollArea->resize(e->size());
        ui->verticalLayoutWidget->resize(e->size());
    }
    QWidget::resizeEvent(e);
}

void GenFileDetailsView::deleteUi()
{
    delete ui->kineticArea;
    ui->kineticArea = NULL;
    delete ui;
    ui = NULL;
    _thumbnail = QPixmap();
}

QString GenFileDetailsView::localFilename(QString dropboxPath)
{
    QString localPath = _appModel->settingValue(SETTINGS_DOWNLOAD_LOCATION).toString();
    QString localFilename = localPath + "/" + dropboxPath;
    if (QFile::exists(localFilename))
        return localFilename;
    return QString("");
}

void GenFileDetailsView::aboutToDeactivate()
{
    _fileDownloader->cancelDownload();
    delete _fileDownloader;
    _fileDownloader = NULL;
    ui->itemProgressBar->hide();
    ui->itemProgressBar->reset();
}

void GenFileDetailsView::startDownload()
{
    const GenFileInfo *fileInfo = _appModel->currentFileInfo();
    if (fileInfo)
    {

        const GenWSRequestInfo *request = GetDropboxRequestById(WSReqFileDownload);
        QString urlBase;
        if (request->_isSecure) {
            urlBase = "https://";
        }
        else {
            urlBase = "http://";
        }
        urlBase += request->_domain + DROPBOX_VERSION + request->_path;
        if (!fileInfo->_path.isEmpty())
        {
            urlBase += fileInfo->_path;
        }

        QUrl url = urlBase;
        _oauthRequest->initRequest(request->_authType, url);
        _oauthRequest->setHttpMethod(request->_httpType);
        _oauthRequest->setConsumerKey(APP_OAUTH_KEY);
        _oauthRequest->setConsumerSecretKey(APP_OAUTH_SECRET);
        _oauthRequest->setToken(_wsEngine->token());
        _oauthRequest->setTokenSecret(_wsEngine->tokenSecret());
        _fileDownloader->download(fileInfo->_path,
                                  fileInfo->_modifiedTime,
                                  _oauthRequest);
        _mainWindow->menuBar()->setMenuButton(GenMenuBar::GenMenuButton1,
                                              ActionNONE,
                                              QPixmap(),
                                              false);
    }
}

void GenFileDetailsView::startCreateThumbnail()
{
    createThumbnail(_localFilename, 256);
}

void GenFileDetailsView::showQRCode()
{
    GenQREncoder *qrEncoder = new GenQREncoder();
    ui->thumbnail->setPixmap(qrEncoder->encodeText(createPublicLink()));
    delete qrEncoder;
}


void GenFileDetailsView::setOpenButton()
{
    _mainWindow->menuBar()->setMenuButton(GenMenuBar::GenMenuButton1,
                                          ActionOPENFILE,
                                          GenResourceReader::pixmapFromResource("open_icon.png"));
}

void GenFileDetailsView::showSendLinkMenu()
{
    if (!_mainWindow->popupOpened())
    {
        QList<QPair<ActionId, QString> > actions;
        actions.append(qMakePair<ActionId, QString>(ActionSENDLINKBYSMS, "By SMS"));
        actions.append(qMakePair<ActionId, QString>(ActionSENDLINKBYEMAIL, "By email"));
        _mainWindow->showPopupMenu("Send link:", actions);
    }
}

void GenFileDetailsView::showEmailAccountSelectionMenu()
{

}

QString  GenFileDetailsView::createPublicLink()
{
    QString link = DROPBOX_PUBLIC_URL + _appModel->getAccountInfo()._uid;
    QString publicPath;
    const GenFileInfo *fi = _appModel->currentFileInfo();
    if (fi)
    {
        publicPath = fi->_path.mid(QString("dropbox/Public").length() + 1);
        QStringList pathParts = publicPath.split("/");
        QStringListIterator it(pathParts);
        publicPath.clear();
        while(it.hasNext())
        {
            publicPath += "/" + QUrl::toPercentEncoding(it.next());
        }
    }
    return link + publicPath;
}

void GenFileDetailsView::copyLinkToClipboard()
{
#ifdef Q_OS_SYMBIAN

    QString text = createPublicLink();
    TPtrC aText (static_cast<const TUint16*>(text.utf16()), text.length());

    //void WriteToClipboardL(RFs &aFs, const TDesC & aText)
    CClipboard *cb = CClipboard::NewForWritingLC(CCoeEnv::Static()->FsSession());

    RStoreWriteStream stream;
    TStreamId stid = stream.CreateLC(cb->Store());
    stream.WriteInt32L(aText.Length());

    TUnicodeCompressor c;
    TMemoryUnicodeSource source(aText.Ptr());
    TInt bytes(0);
    TInt words(0);
    c.CompressL(stream, source, KMaxTInt, aText.Length(), &bytes, &words);

    stream.WriteInt8L(0); // magic command! <img src="http://www.msec.it/blog/wp-includes/images/smilies/icon_smile.gif" alt=":)" class="wp-smiley"> 

    stream.CommitL();
    cb->StreamDictionary().AssignL(KClipboardUidTypePlainText, stid);
    cb->CommitL();

    stream.Close();
    CleanupStack::PopAndDestroy(); // stream.CreateLC
    CleanupStack::PopAndDestroy(cb);
#else
    QApplication::clipboard()->setText(createPublicLink());
#endif
    _mainWindow->showNotificationInfo("Link copied to clipboard");
}

void GenFileDetailsView::sendLinkBySMS()
{
#ifndef Q_OS_WIN32
    QMessage message;
    QMessageAddress::Type addressType = QMessageAddress::Phone;
    message.setType(QMessage::Sms);
    message.setBody(QString(PUBLIC_LINK_BODY).arg(createPublicLink()));
    QMessageService *msgService = new QMessageService(this);
    msgService->compose(message);
    delete msgService;
#endif 
}

void GenFileDetailsView::sendLinkByEmail()
{
#ifndef Q_OS_WIN32
    QMessage message;
    QMessageAddress::Type addressType = QMessageAddress::Email;
    message.setType(QMessage::Email);
    message.setBody(QString(PUBLIC_LINK_BODY).arg(createPublicLink()));
    message.setSubject("Dropbox public link");
    QMessageService *msgService = new QMessageService(this);
    msgService->compose(message);
    delete msgService;
#endif 
}

bool GenFileDetailsView::isImage(QString filename)
{
    QByteArray format = QImageReader::imageFormat(filename);
    if (format.isEmpty())
        return false;
    return true;
}


void GenFileDetailsView::createThumbnail(QString filename, int size)
{
    if (isImage(filename))
    {
        QImageReader imageReader(filename);
        int image_width = imageReader.size().width();
        int image_height = imageReader.size().height();
        if (image_width > image_height) {
            image_height = static_cast<double>(size) / image_width * image_height;
            image_width = size;
        } else if (image_width < image_height) {
            image_width = static_cast<double>(size) / image_height * image_width;
            image_height = size;
        } else {
            image_width = size;
            image_height = size;
        }
        imageReader.setScaledSize(QSize(image_width, image_height));
        QImage thumbnail = imageReader.read();
        if (!thumbnail.isNull())
        {
            _thumbnail = QPixmap::fromImage(thumbnail);
            ui->thumbnail->setPixmap(_thumbnail);
        }
    }
}

#ifndef Q_OS_WIN32
/*
QMessage GenFileDetailsView::composeMessage()
{
    QMessage message;

    if(m_accountsWidget->isEmpty())
    {
        QMessageBox::critical(const_cast<ComposeSendWidget*>(this),"No Accounts","Cannot send a message without any available accounts");
        return message;
    }

    QMessageAccountId selectedAccountId = m_accountsWidget->currentAccount();
    QMessageAccount selectedAccount(selectedAccountId);

    bool composingSms = (selectedAccount.messageTypes() & QMessage::Sms) > 0;

    QMessageAddressList toList;
    QMessageAddressList ccList;
    QMessageAddressList bccList;

    QMessageAddress::Type addressType = QMessageAddress::Email;
    if(composingSms)
    {
        addressType = QMessageAddress::Phone;
        message.setType(QMessage::Sms);
    }

    foreach(QString s, m_toEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
        toList.append(QMessageAddress(addressType, s));
    message.setTo(toList);

    if(!composingSms)
    {
        foreach(QString s, m_ccEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
            ccList.append(QMessageAddress(QMessageAddress::Email, s));
        message.setCc(ccList);

        foreach(QString s, m_bccEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
            bccList.append(QMessageAddress(QMessageAddress::Email, s));
        message.setBcc(bccList);
        message.setSubject(m_subjectEdit->text());

        message.setType(QMessage::Email);

        message.appendAttachments(m_attachmentList->attachments());
    }

    message.setParentAccountId(selectedAccountId);

    if(!composingSms && asHtml) {
        //create html body
        QString htmlBody("<html><head><title></title></head><body><h2 align=center>%1</h2><hr>%2</body></html>");
        message.setBody(htmlBody.arg(message.subject()).arg(m_bodyEdit->toPlainText()),"text/html");
    }
    else
        message.setBody(m_bodyEdit->toPlainText());

    return message;
}
*/
#endif


void GenFileDetailsView::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->itemProgressBar->setMaximum(bytesTotal);
    ui->itemProgressBar->setValue(bytesReceived);
}

void GenFileDetailsView::handleDownloadFinished(QNetworkReply::NetworkError error, int httpStatus, QByteArray reply)
{
    QLOG_DEBUG("GenFileDetailsView::handleDownloadFinished - error: " << error
               << ", httpStaus: " << httpStatus
               << ", reply : " << reply);
    ui->itemProgressBar->hide();
    ui->itemProgressBar->reset();
    if (QNetworkReply::NoError == error && QNetworkReply::OperationCanceledError != error)
    {
        _localUpToDate = true;
        setOpenButton();
        const GenFileInfo *fileInfo = _appModel->currentFileInfo();
        if (fileInfo)
        {
            _localFilename = localFilename(fileInfo->_path);
            if (!_localFilename.isEmpty())
                QTimer::singleShot(VIEW_ANIMATION_DURATION, this, SLOT(startCreateThumbnail()));
        }
    }
}

void GenFileDetailsView::handleDownloadStarted(const QString &filename)
{
    ui->itemProgressBar->show();
}


void GenFileDetailsView::handleFlicked(GenKineticScrollEngine::SwipeDirection direction)
{
    if (GenKineticScrollEngine::Right == direction)
        handleAction(ActionBACK);
}
