#include <QResizeEvent>
#include <QFrame>
#include <QSpacerItem>
#include <QPainter>
#include <QLinearGradient>
#include <QPixmapCache>

#include "genpopupmenu.h"
#include "ui_genpopupmenu.h"
#include "genpushbutton.h"
#include "genconstants.h"

GenPopupMenu::GenPopupMenu(QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenPopupMenu)
{
    ui->setupUi(this);
}

GenPopupMenu::~GenPopupMenu()
{
    delete ui;
}

void GenPopupMenu::setCaption(QString caption)
{
    ui->label->setText(caption);
}

void GenPopupMenu::addActions(QList<QPair<ActionId, QString> > actions)
{
    QListIterator<QPair<ActionId, QString> >  it(actions);
    while(it.hasNext())
    {
        QPair<ActionId, QString> action = it.next();
        addAction(action.first, action.second, it.hasNext());
    }
    ui->actionsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
    resize(minimumSizeHint()/* + ui->actionsLayout->sizeHint()*/);
}

void GenPopupMenu::resizeEvent(QResizeEvent *e)
{
    resize(minimumSizeHint()/* + ui->actionsLayout->sizeHint()*/);
}

void GenPopupMenu::paintEvent(QPaintEvent *e)
{
    QPixmap pixmap;
    QPainter painter(this);
    if (QPixmapCache::find(QString("bla%1").arg((int)this), pixmap))
    {
        painter.drawPixmap(0, 0, pixmap);
    }
    else
    {
        painter.setRenderHint(QPainter::Antialiasing);
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0.0, QColor(88, 88, 93));
        gradient.setColorAt(1.0, QColor(39, 39, 44));
        painter.setBrush(gradient);
        painter.drawRoundedRect(QRect(QPoint(0, 0), size() - QSize(1, 1)), 12.0, 12.0);
    }
}

void GenPopupMenu::addAction(ActionId actionId, QString actionText, bool separator)
{
    GenPushButton *button = new GenPushButton(this);
    button->setObjectName(actionText);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(button->sizePolicy().hasHeightForWidth());
    button->setSizePolicy(sizePolicy);
    button->setMinimumSize(QSize(0, 35));
    QFont font;
    font.setPointSize(8);
    font.setBold(true);
    font.setWeight(75);
    button->setFont(font);
    button->setContextMenuPolicy(Qt::NoContextMenu);
    button->setStyleSheet(QString::fromUtf8("QPushButton\n"
                                            "{\n"
                                            "    background: transparent;\n"
                                            "    border-width: 0px;\n"
                                            "    color: rgb(255,255,255);\n"
                                            "	border-style: none;\n"
                                            "	text-align: left;\n"
                                            "	padding: 0 10;\n"
                                            "}\n"
                                            "\n"
                                            "QPushButton::pressed\n"
                                            "{\n"
                                            "    border-width: 0px;\n"
                                            "	border-style: none;\n"
                                            "	border-radius: 9px;\n"
                                            "	background-color: qlineargradient(spread:pad, x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgba(142, 207, 81, 255), stop:1 rgba(62, 125, 41, 255));\n"
                                            "}"));
    button->setAction(actionId);
    button->setText(actionText);
    connect(button, SIGNAL(clicked(ActionId)), SIGNAL(actionTriggered(ActionId)));
    ui->actionsLayout->addWidget(button);
    if (separator)
    {
        QFrame *line = new QFrame(this);
        line->setObjectName(QString::fromUtf8("line_2"));
        line->setStyleSheet(QString::fromUtf8("background: transparent;\n"
                                              "color: grey;"));
        line->setFrameShadow(QFrame::Plain);
        line->setFrameShape(QFrame::HLine);

        ui->actionsLayout->addWidget(line);
    }
}
