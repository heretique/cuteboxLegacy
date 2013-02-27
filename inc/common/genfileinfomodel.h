#ifndef GENFILEINFOMODEL_H
#define GENFILEINFOMODEL_H

#include <QAbstractListModel>
#include <QFileSystemModel>
#include <QNetworkReply>

class GenFileInfo;
class GenAppModel;

class GenFileInfoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum GenFileInfoRoles
    {

    };

    GenFileInfoModel(GenAppModel& appModel, QObject *parent = 0);
    ~GenFileInfoModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    void setFilesInfo(QList<GenFileInfo*> filesInfo);
    void addFileInfo(GenFileInfo *fileInfo);
    const GenFileInfo *fileInfo(const QModelIndex &index) const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    void startThumbnailGeneration() const;

private:

public slots:
    void handleThumbnailReady(QString path, QImage thumbnail);
    void handleThumbnailError(QString path, QNetworkReply::NetworkError error,
                              int httpStatus,
                              QByteArray reply);

private slots:
    void requestNextThumbnail() const;

signals:
    void requestThumbnail(QString path) const;

private:
    GenAppModel &_appModel;
    QList<GenFileInfo*> _filesInfo;
    mutable QMap<QString, GenFileInfo*> _filesInfoMap;
    mutable bool _thumbRetrievalInProgress;
};

#endif // GENFILEINFOMODEL_H
