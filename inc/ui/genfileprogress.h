#ifndef GENFILEPROGRESS_H
#define GENFILEPROGRESS_H

#include "geninterfaces.h"
#include <QTime>

#include "gentypes.h"

namespace Ui {
    class GenFileProgress;
}

class GenFileProgress : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenFileProgress(QWidget *parent = 0);
    ~GenFileProgress();
    void setText(const QString& text);
    void setItemProgress(qint64 bytes, qint64 bytesTotal);
    void setTotalProgress(int current, int total);
    void showTotalProgress(bool show);
    void start();
    void reset();

signals:
    void fileActionCanceled();

private:
    Ui::GenFileProgress *ui;
    QTime _fileProgressTime;
};

#endif // GENFILEPROGRESS_H
