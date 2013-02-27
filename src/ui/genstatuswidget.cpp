#include "genstatuswidget.h"
#include "ui_genstatuswidget.h"
#include "genindicatorswidget.h"
#include "genresourcereader.h"

GenStatusWidget::GenStatusWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenStatusWidget),
    _waiting(false),
    _showHomeIndicator(false)
{
    ui->setupUi(this);
    ui->indicatorButton->setAction(ActionGOTOROOT);
    ui->exit->setAction(ActionEXIT);
    connect(ui->indicatorButton,
            SIGNAL(clicked(ActionId)),
            SIGNAL(actionTriggered(ActionId)));
    connect(ui->exit,
            SIGNAL(clicked(ActionId)),
            SIGNAL(actionTriggered(ActionId)));
    ui->waiting->setPixmap(QPixmap(QString::fromUtf8(":/images/wait_icon.png")));
    ui->waiting->hide();
    ui->indicatorButton->hide();
    ui->exit->hide();
}

GenStatusWidget::~GenStatusWidget()
{
    delete ui;
    ui = 0;
}

void GenStatusWidget::setText(const QString &text)
{
    _text = text;
    if (!_waiting)
    {
        QLabel *label = ui->text;
        QFontMetrics fontMetrics(label->font());
        ui->text->setText(fontMetrics.elidedText(text, Qt::ElideLeft, label->width()));
    }
}

const GenPushButton *GenStatusWidget::indicatorButton() const
{
    return ui->indicatorButton;
}

void GenStatusWidget::setIndicatorIcon(QPixmap icon)
{
    ui->indicatorButton->setIcon(icon);
}

GenIndicatorsWidget *GenStatusWidget::indicatorsWidget()
{
    if (ui)
        return ui->indicatorsWidget;
    return NULL;
}

void GenStatusWidget::showHomeButton(bool show)
{
    if (!_waiting)
    {
        if (show)
        {
//            ui->indicatorButton->setIconSize(QSize(32, 32));
//            ui->indicatorButton->setEnabled(true);
            ui->indicatorButton->show();
            return;
        }
//        ui->indicatorButton->setIconSize(QSize(0, 0));
//        ui->indicatorButton->setEnabled(false);
        ui->indicatorButton->hide();
    }
    _showHomeIndicator = show;
}

void GenStatusWidget::showExitButton(bool show)
{
    if (show)
    {
//        ui->exit->setIconSize(QSize(16, 16));
//        ui->exit->setEnabled(true);
        ui->exit->show();
        return;
    }
//    ui->exit->setIconSize(QSize(0, 0));
//    ui->exit->setEnabled(false);
    ui->exit->hide();
}

void GenStatusWidget::showWaitingIndicator(QString text)
{
    _waiting = true;
    ui->indicatorButton->hide();
    ui->waiting->show();
    QLabel *label = ui->text;
    QFontMetrics fontMetrics(label->font());
    ui->text->setText(fontMetrics.elidedText(text, Qt::ElideLeft, label->width()));
    ui->waiting->start();
}

void GenStatusWidget::hideWaitingIndicator()
{
    _waiting = false;
    ui->waiting->stop();
    ui->waiting->hide();
    setText(_text);
    showHomeButton(_showHomeIndicator);
}
