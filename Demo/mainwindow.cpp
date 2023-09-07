#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "newdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 创建一个 QLable 对象
//    this->lab = new QLabel("Hello,World!",this);
    ui->pushButton->setText("新窗口"); //将界面上按钮的显示文本更改为“新窗口”

    // 创建新的动作
    QAction *findAction = new QAction(tr("查找(&F)"), this);
    // 添加图标
    QIcon icon(":/myImg/images/lion.png");
    findAction->setIcon(icon);
    // 设置快捷键
    findAction->setShortcut(QKeySequence(tr("Ctrl+F")));
    // 在文件菜单中设置新的打开动作
    ui->menu_F->addAction(findAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    NewDlg *dlg = new NewDlg(this);
    dlg->show();
}

