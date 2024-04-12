#ifndef FILEMODEL_H
#define FILEMODEL_H
#include <QObject>
#include <QStandardItemModel>
#include <QTime>
#include <ftp.h>
#include <sortfilterproxymodel.h>
struct FileInfo {
    QString fileName;
    QString fileType; // 使用文件扩展名作为文件类型
    qint64 fileSize;
    QString lastModified;
    bool isDir;
};
class FileModel : public QStandardItemModel
{
    Q_OBJECT
public:
    SortFilterProxyModel proxyModel;
    Ftp *ftp;
    QList<FileInfo> fileInfoList;
    explicit FileModel(QStandardItemModel *parent = nullptr);
    void refresh();
    void resetModel();
signals:

};



#endif // FILEMODEL_H
