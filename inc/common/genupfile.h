#ifndef GENUPFILE_H
#define GENUPFILE_H

#include <QFile>
#include <QIODevice>

class GenUpFile : public QIODevice
{
    Q_OBJECT
public:
    GenUpFile(const QString & name, const QByteArray &head, const QByteArray &tail, QObject *parent = 0);
    ~GenUpFile();
    bool openFile();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 size() const;

private:
    QFile *_file;
    QByteArray *_head;
    QByteArray *_tail;
    qint64 _position;
    qint64 sizepart() const;
    qint64 sizefull() const;
};

#endif // GENUPFILE_H
