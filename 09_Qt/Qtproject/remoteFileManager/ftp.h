#ifndef FTP_H
#define FTP_H

#include <QObject>
#include <QTcpSocket>
#include <string>
#include <cstdlib>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStateMachine>
#include <QState>
#include <QStandardPaths>

enum OperationType {
    None,
    ListDirectory,
    DownloadFile
};

class Ftp : public QObject
{
    Q_OBJECT
public:
    QString host;
    ushort port;
    QString userName;
    QString passWord;
    QString curDir;  // 当前目录
    QStringList features;
    QString directoryList;  // 目录列表
    QString changeDir;
    QString downloadFile;
    QFile* file;
    char sendCmdBuf[256];
    QTcpSocket *cmdSocket; // 命令套接字
    QTcpSocket *dataSocket; // 数据套接字
    OperationType operationType;
    bool isPasv;



    void login();

    void list();
    void pasv();  // 被动模式
    void retr();
    void delFile();
    void readToDirectoryList();
    void readToLocalFile();


    explicit Ftp(QObject *parent = nullptr);
    ~Ftp();
private:
    bool isLongCmd;
    int cmd;
    char arg[128];
    void cmdStructor(const QString&);  // 命令格式化

signals:
    void connectTriggered();  // 按下连接按钮
    void hostError(); // 未输入ip port


    void connFail(); // 套接字未连接
    void loginError(); // 登录失败  用户名 密码错误
    void loginSuss();  // 连接成功
    void cmdRecvFail(); // 命令接收失败，断开重连
    void listModel();  // 登录成功
    void pasvModel();  // pasv状态
    void chDirModel();  // 切换目录状态
    void listDownloading(); // list 信号
    void fileDownloading();  // 下载文件信号
    void transmissionFinished(); // 文件下载完成
    void refresh();
    void changeDirectorySucc();
    void downloadFileTriggered();

private slots:
    int sendByCmdBuf();  // 发送命令
    void handlePasvResponse();
    void connectToFtp();
    void onCommandSocketReadyRead();
    void onDataSocketDisconnect();
    void listFiles();
    void listDownload();
    void changeDirectory();

    void downloadFiles();
    void fileDownload();
};

#endif // FTP_H
