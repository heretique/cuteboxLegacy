#ifndef GENFILEICONPROVIDER_H
#define GENFILEICONPROVIDER_H

#include <QFileIconProvider>

class GenFileIconProvider : public QFileIconProvider
{
public:
    GenFileIconProvider();
    ~GenFileIconProvider();
    QIcon icon(const QFileInfo &info) const;

private:
    QString iconNameFromExtension(QString extension) const;
};

#endif // GENFILEICONPROVIDER_H
