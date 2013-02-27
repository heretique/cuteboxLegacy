#include <QPainter>
#include <QApplication>
#include "genfileinfodelegate.h"
#include "genresourcereader.h"

const int MAX_VALUE = 100;
const int MIN_BAR_WIDTH = 64;
const int BAR_HEIGHT = 100;

GenFileInfoDelegate::GenFileInfoDelegate(QObject *parent) :
        QItemDelegate(parent)
{
}

void GenFileInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //        // calculate text min width
    //        QString text = index.data().toString();
    //        // draw value text centered
    //        painter->drawText(option.rect, text, QTextOption(Qt::AlignLeft));
    QItemDelegate::paint(painter, option, index);

    QRect itemRect = option.rect;
    QPixmap separator = GenResourceReader::pixmapFromResource("separator");
    int x = itemRect.left() + (itemRect.width() - separator.size().width()) / 2;
    int y = itemRect.bottom() + separator.size().height() + 5;
    painter->drawPixmap(x, y, separator);
}

QSize GenFileInfoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size += QSize(0, GenResourceReader::pixmapFromResource("separator").size().height() + 5);
    return size;
}

void GenFileInfoDelegate::drawDisplay(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QRect &rect,
                                      const QString &text) const
{
    QPen pen = painter->pen();
    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                              ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(rect, option.palette.brush(cg, QPalette::Highlight));
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }

    if (text.isEmpty())
        return;

    if (option.state & QStyle::State_Editing) {
        painter->save();
        painter->setPen(option.palette.color(cg, QPalette::Text));
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        painter->restore();
    }

    const QStyleOptionViewItemV4 opt = option;

    const QWidget *widget = this->widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();
    const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
    QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
    const bool wrapText = opt.features & QStyleOptionViewItemV2::WrapText;
    textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
    textOption.setTextDirection(option.direction);
    textOption.setAlignment(QStyle::visualAlignment(option.direction, option.displayAlignment));
    textLayout.setTextOption(textOption);

    // if it is a folder bold it
    textLayout.setText(replaceNewLine(text));
    bool isDir = text.count(QChar::LineSeparator) == 0;
    QFont font = option.font;
    if (isDir)
        font.setBold(true);
    textLayout.setFont(font);

    QSizeF textLayoutSize = doTextLayout(textRect.width());

    if (textRect.width() < textLayoutSize.width()
        || textRect.height() < textLayoutSize.height())
    {
        QString elided;
        int start = 0;
        int end = text.indexOf(QChar::LineSeparator, start);
        if (end == -1)
        {
            elided += option.fontMetrics.elidedText(text, option.textElideMode, textRect.width());
        }
        else
        {
            while (end != -1)
            {
                elided += option.fontMetrics.elidedText(text.mid(start, end - start),
                                                        option.textElideMode, textRect.width());
                elided += QChar::LineSeparator;
                start = end + 1;
                end = text.indexOf(QChar::LineSeparator, start);
            }
            //let's add the last line (after the last QChar::LineSeparator)
            elided += text.mid(start);
        }
        textLayout.setText(elided);
        textLayoutSize = doTextLayout(textRect.width());
    }

    const QSize layoutSize(textRect.width(), int(textLayoutSize.height()));
    const QRect layoutRect = QStyle::alignedRect(option.direction, option.displayAlignment,
                                                 layoutSize, textRect);
    // add new format for lines beyond firs one
    if (!isDir)
    {
        QTextLayout::FormatRange newLinesFormat;
        newLinesFormat.start = textLayout.text().indexOf(QChar::LineSeparator, 0);
        newLinesFormat.length = textLayout.text().length() - newLinesFormat.start;
        newLinesFormat.format.setFont(textLayout.font());
        newLinesFormat.format.setFontPointSize(textLayout.font().pointSize() - 2);
        QList<QTextLayout::FormatRange> additionalFormats;
        additionalFormats.append(newLinesFormat);
        textLayout.setAdditionalFormats(additionalFormats);
    }
    // if we still overflow even after eliding the text, enable clipping
    if (!hasClipping() && (textRect.width() < textLayoutSize.width()
        || textRect.height() < textLayoutSize.height())) {
        painter->save();
        painter->setClipRect(layoutRect);
        textLayout.draw(painter, layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), layoutRect);
        painter->restore();
    } else {
        textLayout.draw(painter, layoutRect.topLeft(), QVector<QTextLayout::FormatRange>(), layoutRect);
    }
    textLayout.clearAdditionalFormats();
}

QSizeF GenFileInfoDelegate::doTextLayout(int lineWidth) const
{
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}
