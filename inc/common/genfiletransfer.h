#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QThread>
#include <QMutex>


class FileTransfer : public QThread
{
    Q_OBJECT

public:

    enum Error
    {
        NoError,
        SourceDoesntExists,
        DestinationExists,
        SourceDirectoryOmitted,
        SourceFileOmitted,
        PathToDestinationNotExists,
        CannotCreateDestinationDirectory,
        CannotOpenSourceFile,
        CannotOpenDestinationFile,
        CannotRemoveDestinationFile,
        CannotReadSourceFile,
        CannotWriteDestinationFile,
        CannotRemoveSource,
        CannotRenameFile,
        Cancelled
    };

    enum CopyFlag
    {
        DoNothing = 0x00,
        Overwrite = 0x01,
        EnsurePathExists = 0x02,
        MoveFile = 0x04
    };

    Q_DECLARE_FLAGS(CopyFlags, CopyFlag)

    FileTransfer(QObject* parent = 0);
    ~FileTransfer();

    void fileTransfer(QString source, QString destination, CopyFlags flags = 0);
    void cancel();

private:
    void run();
    bool ensurePathExists(QString path);
    bool deleteFile(QString absoluteFilePath);

signals:
    void fileTransferStarted(QString source, QString destination, qint64 size);
    void fileTransferProgress(qint64 copied);
    void fileTransferFinished();
    void fileTransferError(int error, QString description);

private:
    QString _source;
    QString _destination;
    CopyFlags _copyFlags;
    bool _cancel;
    mutable QMutex _mutex;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FileTransfer::CopyFlags)
const int READ_BLOCK_SIZE = 4096;

#endif // FILETRANSFER_H
