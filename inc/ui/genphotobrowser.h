#ifndef _GENPHOTOBROWSER_H_
#define _GENPHOTOBROWSER_H_

#include <QGraphicsView>


class GenPhotoBrowser : public QGraphicsView
{
    Q_OBJECT
public:
    GenPhotoBrowser ( QWidget * parent = 0 );
    virtual ~GenPhotoBrowser();

    void initialize();

protected:
    void resizeEvent(QResizeEvent *event);

private:
};



#endif // _GENPHOTOBROWSER_H_
