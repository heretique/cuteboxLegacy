#ifndef GENROTATINGIMAGE_H
#define GENROTATINGIMAGE_H

#include <QWidget>

class QTimer;

class GenRotatingImage : public QWidget
{
    Q_OBJECT
public:
    GenRotatingImage(QWidget *parent=0);
    ~GenRotatingImage();
    void paintEvent(QPaintEvent *e);
    void setPixmap(QPixmap pixmap);
    void setText(QString t);
    void start();
    void stop();

protected slots:
    void rotateImage();
private:
    QPixmap _pixmap;
    QTimer *_timer;
};

#endif // GENROTATINGIMAGE_H
