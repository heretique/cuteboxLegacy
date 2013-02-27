#include <QResizeEvent>
#include <QAnimationGroup>

#include "genhelpview.h"
#include "ui_genhelpview.h"
#include "genutils.h"
#include "genmainwindow.h"
#include "genstatuswidget.h"
#include "genpageindicator.h"
#include "genappmodel.h"
#include "genliterals.h"
#include "genlog.h"

const QString HELP_IMAGE_FORMAT = ":/images/help/Help-%1.jpg";
const int PAGES_NO = 8;

GenHelpView::GenHelpView(GenAppModel *appModel, GenMainWindow* mainWindow, QWidget *parent) :
    GenBaseView(appModel, mainWindow, parent),
    ui(NULL),
    _newLabel(NULL),
    _animInProgress(false)
{

}

GenHelpView::~GenHelpView()
{
    delete ui;
}

void GenHelpView::deleteUI()
{
    if (ui) {
        delete ui->kineticArea;
        ui->kineticArea = NULL;
        delete ui;
        ui = NULL;
    }
    _images.clear();
}

void GenHelpView::createList()
{
    for (int i = 1; i <= PAGES_NO; ++i)
    {
        _images.append(QString(HELP_IMAGE_FORMAT).arg(i, 2, 10, QLatin1Char('0')));
    }
}

void GenHelpView::activate(const QVariant &data)
{
    deleteUI();
    _mainWindow->useImageBackground(false);
    createList();
    ui = new Ui::GenHelpView;
    ui->setupUi(this);
    ui->currentPage->setEnabled(false);
    ui->kineticArea->enableKineticScrollingFor(ui->scrollArea, Qt::Vertical);
    connect(ui->kineticArea->kineticEngine(), SIGNAL(flicked(GenKineticScrollEngine::SwipeDirection)),
            SLOT(handleFlicked(GenKineticScrollEngine::SwipeDirection)));
    ui->currentImage->setPixmap(QPixmap(_images.at(0)));
    ui->currentPage->setNumPages(_images.size());
    ui->currentPage->setCurrentPage(0);
    _currentImageIndex = 0;
    _mainWindow->statusBar()->setText("Application usage");
}

void GenHelpView::deactivate()
{
    deleteUI();
}

void GenHelpView::updateMenuBar(GenMenuBar &menuBar)
{
    setCancelMenuButton(menuBar);
}

bool GenHelpView::handleAction(const ActionId actionId)
{
    switch(actionId)
    {
    case ActionCANCEL:
        if (_appModel->isFirstRun())
        {
            _appModel->setSettingValue(SETTING_FIRST_RUN, false);
            activateView("GenMainView", QVariant(), true, GenFromBottom);
        }
        else
            activateView("GenSettingsView", QVariant(), true, GenFromTop);
        return true;
    case ActionBACK:
        previousPage();
        break;
    case ActionFORWARD:
        nextPage();
        break;
    }

    return false;
}

void GenHelpView::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if (ui)
    {
        ui->currentImage->resize(e->size());
        ui->currentPage->move(QPoint(0, e->size().height() - ui->currentPage->height()));
        QLOG_DEBUG("GenHelpView::resizeEvent - size = " << e->size());
        QLOG_DEBUG("Page indicator geometry = " << ui->currentPage->geometry());
    }
}

void GenHelpView::previousPage()
{
    if (_animInProgress && !ui)
        return;
    ui->currentPage->previous();
    --_currentImageIndex;
    if (_currentImageIndex > -1)
    {
        changePage(false);
    }
    else
        _currentImageIndex = 0;
}

void GenHelpView::nextPage()
{
    if (_animInProgress && !ui)
        return;
    ui->currentPage->next();
    ++_currentImageIndex;
    if (_currentImageIndex < _images.size())
    {
        changePage(true);
    }
    else
        _currentImageIndex = _images.size() - 1;
}

void GenHelpView::changePage(bool isForward)
{
    _newLabel = new QLabel(ui->scrollAreaWidgetContents);
    _newLabel->setGeometry(QRect(0, 0, 360, 490));
    _newLabel->setAlignment(Qt::AlignCenter);
    _newLabel->setPixmap(QPixmap(_images.at(_currentImageIndex)));
    _newLabel->stackUnder(ui->currentPage);
    _newLabel->hide();
    QAnimationGroup* aniGroup = _mainWindow->createViewAnimation(*(ui->currentImage), *_newLabel,
                                                isForward ? GenFromRight : GenFromLeft, false);
    connect(aniGroup, SIGNAL( finished() ), this,
            SLOT( changeAnimationFinished()));
    aniGroup->start(QAbstractAnimation::DeleteWhenStopped);
    _animInProgress = true;
}

void GenHelpView::handleFlicked(GenKineticScrollEngine::SwipeDirection direction)
{
    if (GenKineticScrollEngine::Right == direction)
        handleAction(ActionBACK);
    else if (GenKineticScrollEngine::Left == direction)
        handleAction(ActionFORWARD);
}

void GenHelpView::changeAnimationFinished()
{
    QLOG_DEBUG("GenHelpView::changeAnimationFinished - _newLabel geom = " << _newLabel->geometry());
    delete ui->currentImage;
    ui->currentImage = _newLabel;
    _newLabel->setEnabled(true);
    _animInProgress = false;
    _newLabel = NULL;
}

