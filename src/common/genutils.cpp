#include <QCryptographicHash>

#include "genutils.h"
#include "genmenubar.h"
#include "genresourcereader.h"

QString FileSize(int size)
{
    QString suffix;
    qreal pointSize = 0;
    if (size < 1024)
    {
        suffix = " bytes";
        pointSize = size;
    }
    else if (size < 1048576)
    {
        suffix = " KB";
        pointSize = size / 1024.0;
    }
    else
    {
        suffix = " MB";
        pointSize = size / 1048576;
    }
    return QString().setNum(pointSize, 'f', 1).append(suffix);
}

QString Duration(QDateTime dateTime)
{
    QDateTime currentDate = QDateTime::currentDateTime().toUTC();
    //    QLOG_DEBUG("duration - current time: " << currentDate << ", file time: " << dateTime);

    static int seconds = 0;
    static int minutes = 0;
    static int hours = 0;
    static int days = 0;
    static int months = 0;
    static int years = 0;

    seconds = dateTime.secsTo(currentDate);
    minutes = seconds / 60;
    hours = seconds / 3600;
    days = hours / 24;
    months = days / 30;
    years = days / 365;

    if (years)
    {
        if (years == 1)
            return QString("modified 1 year ago");
        return QString("modified %1 years ago").arg(years);
    }
    else if (months)
    {
        if (months == 1)
            return QString("modified 1 month ago");
        return QString("modified %1 months ago").arg(months);
    }
    else if (days)
    {
        if (days == 1)
            return QString("modified 1 day ago");
        return QString("modified %1 days ago").arg(days);
    }
    else if (hours)
    {
        if (hours == 1)
            return QString("modified 1 hour ago");
        return QString("modified %1 hours ago").arg(hours);
    }
    else if (minutes)
    {
        if (minutes == 1)
            return QString("modified 1 minute ago");
        return QString("modified %1 minutes ago").arg(minutes);
    }
    else if (seconds)
    {
        return QString("modified %1 seconds ago").arg(seconds);
    }
    return QString();
}


void setCancelMenuButton(GenMenuBar &menuBar)
{
    menuBar.setMenuActions(ActionNONE,
                           ActionNONE,
                           ActionNONE,
                           ActionCANCEL);
    menuBar.setMenuIcons(QPixmap(),
                         QPixmap(),
                         QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton4, GenMenuBar::GenMenuButton4);
}

void setOKCancelMenuButtons(GenMenuBar &menuBar)
{
    menuBar.setMenuActions(ActionOK,
                           ActionNONE,
                           ActionNONE,
                           ActionCANCEL);
    menuBar.setMenuIcons(GenResourceReader::pixmapFromResource("ok_icon.png"),
                         QPixmap(),
                         QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton1 |
                           GenMenuBar::GenMenuButton4,
                           GenMenuBar::GenMenuButton1 |
                           GenMenuBar::GenMenuButton4);
}

void setExitMenuButton(GenMenuBar &menuBar)
{
    menuBar.setMenuActions(ActionNONE,
                           ActionNONE,
                           ActionNONE,
                           ActionEXIT);
    menuBar.setMenuIcons(QPixmap(),
                         QPixmap(),
                         QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton4, GenMenuBar::GenMenuButton4);
}

void setSettingsMenuButtons(GenMenuBar &menuBar)
{
    menuBar.setMenuActions(ActionHELP,
                           ActionNONE,
                           ActionNONE,
                           ActionCANCEL);
    menuBar.setMenuIcons(GenResourceReader::pixmapFromResource("help_icon.png"),
                         QPixmap(),
                         QPixmap(),
                         GenResourceReader::pixmapFromResource("cancel_icon.png"));
    menuBar.setMenuButtons(GenMenuBar::GenMenuButton1 |
                           GenMenuBar::GenMenuButton4,
                           GenMenuBar::GenMenuButton1 |
                           GenMenuBar::GenMenuButton4);
}

uint thumbnailHash(QString path)
{
    return qHash(path);
}
