#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "newdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 创建一个 QLable 对象
    this->lab = new QLabel("Hello,World!",this);
    ui->pushButton->setText("新窗口"); //将界面上按钮的显示文本更改为“新窗口”
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

