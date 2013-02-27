#ifndef GENFILEINFODELEGATE_H
#define GENFILEINFODELEGATE_H

#include <QItemDelegate>
#include <QTextLayout>
#include <QTextOption>


class GenFileInfoDelegate : public QItemDelegate
{
public:
    explicit GenFileInfoDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    virtual void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                             const QRect &rect, const QString &text) const;

    QSizeF doTextLayout(int lineWidth) const;

    const QWidget *widget(const QStyleOptionViewItem &option) const
    {
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
            return v3->widget;

        return 0;
    }

    inline static QString replaceNewLine(QString text)
        {
            const QChar nl = QLatin1Char('\n');
            for (int i = 0; i < text.count(); ++i)
                if (text.at(i) == nl)
                    text[i] = QChar::LineSeparator;
            return text;
        }

private:
    mutable QTextLayout textLayout;
    mutable QTextOption textOption;
};


#endif // GENFILEINFODELEGATE_H
