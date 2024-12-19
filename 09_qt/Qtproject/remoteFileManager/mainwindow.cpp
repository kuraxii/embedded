#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    treeViewInit();


    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::login);

    connect(fileModel.ftp, &Ftp::hostError, [=](){
        qDebug() << "host错误";
    });
    connect(fileModel.ftp, &Ftp::loginSuss, [=](){
        qDebug() << "登录成功";
    });
    connect(fileModel.ftp, &Ftp::loginError, [=](){
        qDebug() << "登录失败";
    });
}

MainWindow::~MainWindow()
{

    delete ui;

}

void MainWindow::treeViewInit()
{
    // 初始化model

    QStringList headList;
    headList << "FileName" << "Size" << "Type" << "Date Modified";
    fileModel.setHorizontalHeaderLabels(headList);
    fileModel.proxyModel.setSourceModel(&fileModel);
    ui->treeView->setModel(&fileModel.proxyModel);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewdoubleClicked);


}

void MainWindow::showContextMenu(const QPoint &pos) {
    QPoint globalPos = ui->treeView->viewport()->mapToGlobal(pos);
    QMenu menu;
    menu.addAction("下载", this, SLOT(onActionDownloadTriggered()));
//    menu.addSeparator();
//    menu.addAction("创建文件", this, SLOT(onActionCreateFileTriggered()));
//    menu.addAction("创建文件夹", this, SLOT(onActionCreateDirTriggered()));
//    menu.addAction("删除", this, SLOT(onActionDeleteTriggered()));

    menu.exec(globalPos);
}

void MainWindow::onActionDeleteTriggered() {
    QModelIndex currentIndex = ui->treeView->currentIndex(); // 获取当前选中的项
    int row = currentIndex.row(); // 获取行号
    QModelIndex firstColumnIndex = currentIndex.model()->index(row, 0); // 获取第一列的QModelIndex
    QString itemName = currentIndex.model()->data(firstColumnIndex, Qt::DisplayRole).toString(); // 获取文本
    qDebug() << "onActionDeleteTriggered   当前选择的TreeView项的名称为:" << itemName;
    // 接下来可以根据itemName来进行删除等操作
}

void MainWindow::onActionDownloadTriggered()
{
    QModelIndex currentIndex = ui->treeView->currentIndex(); // 获取当前选中的项
    int row = currentIndex.row(); // 获取行号
    QModelIndex typeColumnIndex = currentIndex.model()->index(row, 2); // 获取类型
    QString type = currentIndex.model()->data(typeColumnIndex, Qt::DisplayRole).toString();
    if(type == "文件夹")
    {
        QMessageBox::critical(this, "critical", "文件夹不可下载!");
        return;
    }
    QModelIndex nameColumnIndex = currentIndex.model()->index(row, 0); // 获取名字
    QString name = currentIndex.model()->data(nameColumnIndex, Qt::DisplayRole).toString(); // 获取文本

    fileModel.ftp->downloadFile = name;
    emit fileModel.ftp->downloadFileTriggered();
}

void MainWindow::onActionCreateFileTriggered()
{
    QModelIndex currentIndex = ui->treeView->currentIndex(); // 获取当前选中的项
    int row = currentIndex.row(); // 获取行号
    QModelIndex firstColumnIndex = currentIndex.model()->index(row, 0); // 获取第一列的QModelIndex
    QString itemName = currentIndex.model()->data(firstColumnIndex, Qt::DisplayRole).toString(); // 获取文本
    qDebug() << "当前选择的TreeView项的名称为:" << itemName;
}

void MainWindow::onActionCreateDirTriggered()
{
    QModelIndex currentIndex = ui->treeView->currentIndex(); // 获取当前选中的项
    int row = currentIndex.row(); // 获取行号
    QModelIndex firstColumnIndex = currentIndex.model()->index(row, 0); // 获取第一列的QModelIndex
    QString itemName = currentIndex.model()->data(firstColumnIndex, Qt::DisplayRole).toString(); // 获取文本

    qDebug() << "当前选择的TreeView项的名称为:" << itemName;
}

void MainWindow::onTreeViewdoubleClicked()
{
    QModelIndex currentIndex = ui->treeView->currentIndex(); // 获取当前选中的项
    int row = currentIndex.row(); // 获取行号
    QModelIndex fileNameColumnIndex = currentIndex.model()->index(row, 0); // 获取第0列的QModelIndex
    QString fileName = currentIndex.model()->data(fileNameColumnIndex, Qt::DisplayRole).toString(); // 获取文本
    QModelIndex fileTypeColumnIndex = currentIndex.model()->index(row, 2); // 获取第2列的QModelIndex
    QString fileType = currentIndex.model()->data(fileTypeColumnIndex, Qt::DisplayRole).toString(); // 获取文本
    if(fileType == "文件夹")
    {
        fileModel.ftp->changeDir = fileName;
        emit fileModel.ftp->chDirModel();
    }

    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QProcess::execute("ffplay -x 480 " + location + "/" + fileName);

}

void MainWindow::login()
{
    fileModel.ftp->userName = ui->userLineEdit->text();
    fileModel.ftp->passWord = ui->passWdEdit->text();
    fileModel.ftp->host = ui->hostEdit->text();
    fileModel.ftp->port = ui->portEdit->text().toUShort();

    emit fileModel.ftp->connectTriggered();
}



