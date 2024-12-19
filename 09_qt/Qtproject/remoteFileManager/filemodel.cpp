#include "filemodel.h"

FileModel::FileModel(QStandardItemModel *parent) : QStandardItemModel(parent)
{
    ftp = new Ftp(this);


    connect(ftp, &Ftp::refresh, this, &FileModel::refresh);

}


void FileModel::refresh()
{
    fileInfoList.clear();
    QStringList lines = ftp->directoryList.split("\r\n");

    foreach (const QString& line, lines) {
        if (line.isEmpty()) continue;

        QStringList parts = line.split(" ", QString::SkipEmptyParts);
        if (parts.size() < 9) continue; // 确保行包含所有必要的部分

        FileInfo info;
        info.fileName = parts[8];
        for (int i = 9; i < parts.size(); ++i) { // 处理文件名中可能包含空格的情况
            info.fileName += " " + parts[i];
        }

        // 文件类型基于扩展名
        int dotIndex = info.fileName.lastIndexOf(".");
        if (dotIndex != -1) {
            info.fileType = info.fileName.mid(dotIndex) + "文件";
        } else {
            info.fileType = "N/A"; // 对于没有扩展名的文件或目录
        }
        if(parts[0].at(0) == 'd')
        {
            info.fileType = "文件夹";
            info.isDir = true;
        }

        info.fileSize = parts[4].toLongLong();

        // 解析最后修改时间

        info.lastModified = parts[5] + " " + parts[6] + " " + parts[7];

        fileInfoList << info;
    }

    resetModel();
}

void FileModel::resetModel()
{
    this->removeRows(0, this->rowCount());

    foreach (const FileInfo& fileInfo, fileInfoList) {
        QList<QStandardItem*> items;
        QStandardItem* temp;
        // 文件名
        temp = new QStandardItem(fileInfo.fileName);
        temp->setEditable(false);
        items.append(temp);

        // 文件大小，，并转换为字符串显示
        temp = new QStandardItem(QString::number(fileInfo.fileSize) + " Byte");
        temp->setEditable(false);
        items.append(temp);

        // 文件类型
        temp = new QStandardItem(fileInfo.fileType);
        temp->setEditable(false);
        items.append(temp);

        // 修改日期，转换为适合显示的格式
        temp = new QStandardItem(fileInfo.lastModified);
        temp->setEditable(false);
        items.append(temp);

        // 将这一行项目添加到模型中
        appendRow(items);
        proxyModel.sort(2, Qt::DescendingOrder); // 假设你想根据文件类型排序

    }
}
