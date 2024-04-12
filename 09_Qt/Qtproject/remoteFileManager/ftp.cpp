#include "ftp.h"
#include <QDebug>
#include <QNetworkProxy>
Ftp::Ftp(QObject *parent) : QObject(parent)
{
    this->cmdSocket = new QTcpSocket(this);
    this->dataSocket = new QTcpSocket(this);
    cmdSocket->setProxy(QNetworkProxy::NoProxy);
    dataSocket->setProxy(QNetworkProxy::NoProxy);





    QStateMachine *machine = new QStateMachine(this);

    // 定义状态
    QState *waitState = new QState(machine);  // 等待状态  连接  下载  ls  cd
    QState *connectState = new QState(machine); // 连接状态
    QState *loginState = new QState(machine);  // 登录状态
    QState *pasvState = new QState(machine);  // pasv状态
    QState *listState = new QState(machine);  // ls状态
    QState *listDownloadState = new QState(machine);  // list下载状态
    QState *downloadState = new QState(machine); // 文件下载状态
    QState *fileDownloadState = new QState(machine);
    QState *changeDirState = new QState(machine); // 切换目录
    QState *refreshState = new QState(machine);  // 刷新

    // 设置状态转换
    waitState->addTransition(this, &Ftp::connectTriggered, connectState); // 按下连接按钮触发连接状态
    waitState->addTransition(this, &Ftp::chDirModel, changeDirState);
    waitState->addTransition(this, &Ftp::downloadFileTriggered, downloadState);
    connectState->addTransition(cmdSocket, &QTcpSocket::connected, loginState);  //  连接成功,触发登录状态
    connectState->addTransition(this, &Ftp::connectTriggered, connectState);     //  重复点击连接按钮
    loginState->addTransition(this, &Ftp::listModel, listState);                     //  登录成功  进入list 状态
    loginState->addTransition(cmdSocket, &QTcpSocket::disconnected, connectState);   // 在登录状态时, 连接断开,重新进入连接状态
    listState->addTransition(this, &Ftp::pasvModel, pasvState);                      // 进入pasv模式
    listState->addTransition(this, &Ftp::listDownloading, listDownloadState);        // 进入listdown状态下载目录文件
    downloadState->addTransition(this, &Ftp::pasvModel, pasvState);
    downloadState->addTransition(this, &Ftp::listDownloading, fileDownloadState);
    pasvState->addTransition(this, &Ftp::listDownloading, listState);
    pasvState->addTransition(this, &Ftp::fileDownloading, downloadState);
    listDownloadState->addTransition(this, &Ftp::transmissionFinished, refreshState); // 文件目录下载完成将进入刷新状态
    fileDownloadState->addTransition(this, &Ftp::transmissionFinished, waitState);
    changeDirState->addTransition(this, &Ftp::changeDirectorySucc, listState);        // 目录切换完成，进入list状态刷新目录
    refreshState->addTransition(this, &Ftp::refresh, waitState); // 刷新完  回到wait状态

    // 定义每个状态的行为
    connect(waitState, &QState::entered, [=](){
       qDebug() << "waitState...";
    });
    connect(connectState, &QState::entered, this, &Ftp::connectToFtp);
    connect(loginState, &QState::entered, this, &Ftp::login);
    connect(listState, &QState::entered, this, &Ftp::listFiles);
    connect(listDownloadState, &QState::entered, this, &Ftp::listDownload);
    connect(downloadState, &QState::entered, this, &Ftp::downloadFiles);
    connect(fileDownloadState, &QState::entered, this, &Ftp::fileDownload);

    connect(pasvState, &QState::entered, this, &Ftp::pasv);

    connect(refreshState, &QState::entered, this, &Ftp::refresh);
    connect(refreshState, &QState::entered, this, &Ftp::refresh);  // 关闭连接
    connect(changeDirState, &QState::entered, this, &Ftp::changeDirectory);

    // 开始状态机
    machine->setInitialState(waitState);
    machine->start();


    connect(cmdSocket, &QTcpSocket::readyRead, this, &Ftp::onCommandSocketReadyRead);

    connect(dataSocket, &QTcpSocket::disconnected, this, &Ftp::onDataSocketDisconnect);  // 断开连接



}

Ftp::~Ftp()
{


}




void Ftp::connectToFtp()
{

    cmdSocket->abort();


    qDebug() << "connectState...";
    if(this->host.isEmpty() || this->port == 0 || userName.isEmpty() || passWord.isEmpty())
    {
        emit hostError();
        return;
    }
    cmdSocket->connectToHost(host, port);

}




void Ftp::login()
{
    qDebug() << "loginState...";
    if(cmdSocket->state() == QAbstractSocket::ConnectedState)
    {
        sprintf(sendCmdBuf, "USER %s\r\n", userName.toStdString().c_str());
        sendByCmdBuf();
    }

}



void Ftp::listFiles()
{
    qDebug() << "listFiles...";
    if(!isPasv)
    {
        qDebug() << "listFiles noPasv...";
        operationType = ListDirectory;
        emit pasvModel();  // 进入pasv模式
    }else{
        qDebug() << "listFiles isPasv";
        list();
        emit fileDownloading();
    }

}

void Ftp::pasv()
{
    qDebug() << "pasvState...";
    sprintf(sendCmdBuf, "PASV\r\n");
    sendByCmdBuf();
}




void Ftp::list()
{

    sprintf(sendCmdBuf, "LIST\r\n");
    sendByCmdBuf();

}

void Ftp::readToDirectoryList()
{
    directoryList = dataSocket->readAll();
}

void Ftp::listDownload()
{
    qDebug() << "listDownloadState...";
    connect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToDirectoryList);
}



void Ftp::downloadFiles()
{
    qDebug() << "downloadFiles...";
    if(!isPasv)
    {
        qDebug() << "downloadFiles noPasv...";
        operationType = DownloadFile;
        emit pasvModel();  // 进入pasv模式
    }else{
        qDebug() << "downloadFiles isPasv";
        retr();
        emit listDownloading();
    }
}

void Ftp::retr()
{
    sprintf(sendCmdBuf, "RETR %s\r\n", downloadFile.toStdString().c_str());
    sendByCmdBuf();
}

void Ftp::fileDownload()
{
    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);



    file = new QFile(location + "/" + downloadFile);
    file->open(QIODevice::ReadWrite);


    connect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToLocalFile);
}

void Ftp::readToLocalFile()
{
    QByteArray fileData = dataSocket->readAll();
    file->write(fileData);

}





// 切换目录
void Ftp::changeDirectory()
{
    qDebug() << "changeDirState...";
    sprintf(sendCmdBuf, ("CWD " + changeDir +"\r\n").toStdString().c_str());
    sendByCmdBuf();
}



void Ftp::handlePasvResponse() {
    QString ip;
    QString response = arg;
    QRegularExpression re("\\((\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)\\)");
    QRegularExpressionMatch match = re.match(response);

    if (match.hasMatch()) {
        // 提取IP地址部分
        ip = QString("%1.%2.%3.%4")
                .arg(match.captured(1))
                .arg(match.captured(2))
                .arg(match.captured(3))
                .arg(match.captured(4));

        // 计算端口号
        int port = match.captured(5).toInt() * 256 + match.captured(6).toInt();

        qDebug() << "IP Address:" << ip << ", Port:" << port;

        dataSocket->abort();
        dataSocket->connectToHost(ip, port);

        isPasv = true;
        qDebug() << "pasv set succ....";
        if(operationType == ListDirectory)
        {
            emit listDownloading();
        }else if(operationType == DownloadFile)
        {
            emit fileDownloading();
        }
    }

}



void Ftp::onCommandSocketReadyRead()
{
    QString responses = cmdSocket->readAll();
    QStringList lines = responses.split("\r\n", QString::SkipEmptyParts);
    qDebug() << "recv: " << responses;

    for (const QString& line : lines) {
        cmdStructor(line);
        qDebug() << "arg:" << arg;
        switch (cmd) {
        case 125:
            break;
        case 150:  // 开始发送文件
            if(operationType == ListDirectory)
            {
                emit listDownloading();
            }else if(operationType == DownloadFile)
            {
                emit fileDownloading();
            }
            break;
        case 200:
            break;
        case 221:  // 服务器特性
            break;
        case 215:   // 系统信息
            break;
        case 226:  // 传输完成
            if(operationType == ListDirectory)
            {
                disconnect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToDirectoryList);
            }else if(operationType == DownloadFile)
            {
                disconnect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToLocalFile);
                file->close();
                delete file;
                file = nullptr;
                qDebug() << "file close";
            }
            dataSocket->abort();
            isPasv = false;
            emit transmissionFinished(); // 传输完成信号
            break;
        case 227:
            handlePasvResponse();  // pasv模式 连接数据套接字

            break;
        case 230: // 登录成功，拥有访问权
            emit this->listModel();  // 发送信号  登录成功
            emit this->loginSuss();
            break;
        case 250:  // 切换目录
            emit changeDirectorySucc();
            break;

        case 257:  // pwd 257 "/home/zzj"
            curDir = arg;
            curDir.remove('\"');
            break;
        case 331: // 客户端接收到用户名  发送密码

            sprintf(sendCmdBuf, "PASS %s\r\n", passWord.toStdString().c_str());
            sendByCmdBuf();
            break;
        case 504:
            break;
        case 530: // 失败  显示cmd arg的失败信息
        case 550:


        default:
            qDebug() << "不识别的代码";
            break;
        }
    }
}

void Ftp::onDataSocketDisconnect()
{
//    if(operationType == ListDirectory)
//    {
//        disconnect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToDirectoryList);
//    }else if(operationType == DownloadFile)
//    {
//        disconnect(dataSocket, &QTcpSocket::readyRead, this, &Ftp::readToDirectoryList);
//        file->close();
//        delete file;
//        file = nullptr;
//    }
}



int Ftp::sendByCmdBuf()
{
    qDebug() << "send:" << sendCmdBuf;
    if(cmdSocket->write(sendCmdBuf) == -1)
    {
        qDebug() << "输入错误" ;
    }
    return 0;
}


void Ftp::cmdStructor(const QString& response)
{

    if (response.isEmpty()) return;

    // 检查是否为211类型的命令
    if(isLongCmd)
    {
        features.append(response.section("\r\n", 0, 0).remove(' '));
    }
    if (response.startsWith("211-")) {
        // 创建一个容器来存放解析出的特性
        isLongCmd = true;
    } else if(response.startsWith("211 End")){
        isLongCmd = false;
    }else {
        // 对于非211类型的命令，提取响应代码并处理

        cmd = response.section(' ', 0, 0, QString::SectionSkipEmpty).toInt(); // 提取响应代码
        memset(arg, 0, sizeof(arg));
        strcpy(arg,response.section(' ', 1, -1, QString::SectionSkipEmpty).toStdString().c_str());  // 提取附加参数
    }

}
