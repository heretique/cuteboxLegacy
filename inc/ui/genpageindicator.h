#ifndef GENPAGEINDICATOR_H
#define GENPAGEINDICATOR_H

#include "geninterfaces.h"

namespace Ui {
    class GenPageIndicator;
}

class QRadioButton;

class GenPageIndicator : public GenCachedWidget
{
    Q_OBJECT

public:
    explicit GenPageIndicator(QWidget *parent = 0);
    ~GenPageIndicator();
    void setNumPages(int pages);
    void setCurrentPage(int page);
    void next();
    void previous();

protected:
    int getCurrentPage() const;

private:
    Ui::GenPageIndicator *ui;
    QList<QRadioButton*> _pages;
};

#endif // GENPAGEINDICATOR_H
