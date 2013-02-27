#include <QPainter>
#include "genlistview.h"

GenListView::GenListView(QWidget *parent)
    :QListView(parent)
{

}

GenListView::~GenListView()
{

}

void GenListView::paintEvent(QPaintEvent *e)
{
    if (model() && !model()->rowCount())
    {
        QPainter painter(viewport());
        painter.setPen(QColor(92, 95, 97));
        QStyleOptionViewItem options = viewOptions();
        QFont font = options.font;
        font.setBold(true);
        painter.save();
        painter.setFont(font);
        painter.drawText(rect(), "(empty)", QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        painter.restore();
    }
    QListView::paintEvent(e);
}
