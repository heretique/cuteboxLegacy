#include "genupfile.h"
#include "genlog.h"


GenUpFile::GenUpFile(const QString &name, const QByteArray &head, const QByteArray &tail, QObject *parent) : QIODevice(parent)
{
    _file = new QFile(name, this);
    _head = new QByteArray(head);
    _tail = new QByteArray(tail);
    _position = 0;
}
GenUpFile::~GenUpFile()
{
    delete _head;
    delete _tail;
}

//////////////////////////////pure virtual//////////////////////////////////////////
qint64 GenUpFile::readData(char *data, qint64 maxlen)
{
    if (!_file->isOpen())
    {
        return -1;
    }
    char *pointer = data;
    qint64 atAll = 0;
    if ((_position<_head->size()) && (maxlen>0))
    {
        qint64 count = qMin(maxlen, (qint64)_head->size());
        memcpy(pointer, _head->data(), count);
        pointer += count;
        _position+=count;
        atAll+=count;
        maxlen -= count;
    }
    if (maxlen>0 && (_position<sizefull()))
    {
        qint64 count = qMin(maxlen, _file->bytesAvailable());
        int s = _file->read(pointer, count);
        pointer+=s;
        maxlen -= s;
        _position+=s;
        atAll+=s;
    }
    if (_position>=sizepart() && (maxlen>0) && (_position<sizefull()))
    {
        qint64 count = qMin(maxlen, (qint64)_tail->size());
        memcpy(pointer, _tail->data(), count);
        _position+=count;
        atAll+=count;
    }
    return atAll;
}

qint64 GenUpFile::writeData(const char *, qint64)
{
    return -1;
}

qint64 GenUpFile::size() const
{
    return sizefull();
}

/////////////////////////////////public//////////////////////////////////
bool GenUpFile::openFile()
{
    if (_file->open(QIODevice::ReadOnly))
        return this->open(QIODevice::ReadOnly);
    return false;
}
//////////////////////////////private////////////////////////////////////////////
qint64 GenUpFile::sizefull() const
{
    return _file->size()+_head->size()+_tail->size();
}

qint64 GenUpFile::sizepart() const
{
    return _head->size()+_file->size();
}

