#ifndef GENUTILS_H
#define GENUTILS_H

#include <QString>
#include <QDateTime>

class GenMenuBar;

QString FileSize(int size);
QString Duration(QDateTime dateTime);

void setCancelMenuButton(GenMenuBar &menuBar);
void setOKCancelMenuButtons(GenMenuBar &menuBar);
void setExitMenuButton(GenMenuBar &menuBar);
void setSettingsMenuButtons(GenMenuBar &menuBar);
uint thumbnailHash(QString path);

#endif // GENUTILS_H
