#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ftp.h>
#include <QStandardItemModel>
#include <filemodel.h>
#include <sortfilterproxymodel.h>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FileModel fileModel;

    QProcess proc;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void treeViewInit();
    void updateTreeView(QStringList);
    void showContextMenu(const QPoint &pos);
private:

    Ui::MainWindow *ui;



private slots:
    void login();
    void onActionDeleteTriggered();
    void onActionDownloadTriggered();

    void onActionCreateFileTriggered();
    void onActionCreateDirTriggered();
    void onTreeViewdoubleClicked();

};
#endif // MAINWINDOW_H
