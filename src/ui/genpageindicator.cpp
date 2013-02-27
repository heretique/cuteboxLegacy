#include <QRadioButton>

#include "genpageindicator.h"
#include "ui_genpageindicator.h"

GenPageIndicator::GenPageIndicator(QWidget *parent) :
    GenCachedWidget(parent),
    ui(new Ui::GenPageIndicator)
{
    ui->setupUi(this);
}

GenPageIndicator::~GenPageIndicator()
{
    delete ui;
}


void GenPageIndicator::setNumPages(int pages)
{
    qDeleteAll(_pages);
    _pages.clear();
    if (pages)
    {
        QRadioButton *button = NULL;
        ui->horizontalLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        for (int i = 0;  i < pages; ++i)
        {
            button = new QRadioButton(this);
            _pages.append(button);
            button->setMaximumSize(QSize(8, 8));
            button->setFocusPolicy(Qt::NoFocus);
            button->setContextMenuPolicy(Qt::NoContextMenu);
            button->setStyleSheet(QString::fromUtf8("QRadioButton {\n"
                                                    "    background: transparent;\n"
                                                    "}\n"
                                                    "QRadioButton::indicator {\n"
                                                    "    width: 8px;\n"
                                                    "    height: 8px;\n"
                                                    "}\n"
                                                    "QRadioButton::indicator::unchecked {\n"
                                                    "    image: url(:/images/radio_icon.png);\n"
                                                    "}\n"
                                                    "QRadioButton::indicator::checked {\n"
                                                    "    image: url(:/images/radio_selection_icon.png);\n"
                                                    "}"));
            ui->horizontalLayout->addWidget(button);
        }
        ui->horizontalLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }
}

void GenPageIndicator::setCurrentPage(int page)
{
    if (page < _pages.size())
    {
        _pages.at(page)->setChecked(true);
    }
}

void GenPageIndicator::next()
{
    int currentpage = getCurrentPage();
    if (-1 != currentpage && ++currentpage < _pages.size())
        setCurrentPage(currentpage);
}

void GenPageIndicator::previous()
{
    int currentpage = getCurrentPage();
    if (-1 != currentpage && --currentpage > -1)
        setCurrentPage(currentpage);
}

int GenPageIndicator::getCurrentPage() const
{
    int i;
    for (i = 0; i < _pages.size(); ++i)
        if (_pages.at(i)->isChecked())
            break;
    if (i < _pages.size())
        return i;

    return -1;
}
