#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMenu();
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // 初始化文件为未保存状态
    isUntitled = true;
    // 初始化文件名为"未命名.txt"
    curFile = "未命名.txt";
    // 初始化窗口标题为文件名
    setWindowTitle(curFile);

    initEvent();
}

void MainWindow::createMenu()
{
    const char* conf = ":/conf/conf_menu.json";
    // 读取JSON文件
    QFile file(conf);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "config file not exist : " << conf;
        return;
    }

    // 解析JSON数据
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        return;
    }

    menubar = new QMenuBar(this);
    this->setMenuBar(menubar);
    toolbar = new QToolBar(this);
    this->addToolBar(toolbar);
    statusbar = new QStatusBar(this);            //创建一个状态栏
    this->setStatusBar(statusbar);

    // 读取配置项
    QJsonArray arr = doc.array();
    foreach (auto menu, arr) {
        createMenuBtn(menu);
    }
}

void MainWindow::createMenuBtn(QJsonValue& menuConf)
{
    QString name = menuConf["name"].toString();
    QString txt = menuConf["text"].toString();
    QJsonArray actions = menuConf["actions"].toArray();

    QMenu *menu = new QMenu(txt, this);
    menubar->addMenu(menu);
    menu->setObjectName(name);

    bool toolbarSp = true;
    foreach (QJsonValue action, actions) {
        createMenuActionBtn(menu, action, toolbarSp);
    }
    // 如果有action添加到toolbar, action本身不带分割线，则添加分割线以区分menu
    if(!toolbarSp)
        toolbar->addSeparator();
}

void MainWindow::createMenuActionBtn(QMenu* menu, QJsonValue& actionConf, bool& toolbarSeparator)
{
    QString name = actionConf["name"].toString();
    QString txt = actionConf["text"].toString();
    QString iconPath = actionConf["iocn"].toString();
    QString hotkey = actionConf["hotKey"].toString();
    bool addToolbar = actionConf["toolbar"].toBool(false);
    bool separator = actionConf["separator"].toBool(false);
    QString status = actionConf["status"].toString();

    // 创建新的动作
    QAction *actionBtn = new QAction(txt, this);
    actionBtn->setObjectName(name);
    // 添加图标
    QIcon icon(iconPath);
    actionBtn->setIcon(icon);
    // 设置快捷键
    actionBtn->setShortcut(QKeySequence(hotkey));
    //状态栏
    actionBtn->setStatusTip(status);
    // 在文件菜单中设置新的打开动作
    menu->addAction(actionBtn);
    // 菜单栏添加分隔符
    if(separator)
    {
        menu->addSeparator();
    }

    // 添加到工具栏
    if(addToolbar)
    {
        toolbar->addAction(actionBtn);
        if(separator)
        {
            toolbar->addSeparator();
        }
        toolbarSeparator = separator;
    }

    // 连接槽
    connect(actionBtn, &QAction::triggered, this, [this, name](bool trigger) {
        onActionClick(trigger, name);
    });
}

void MainWindow::onActionClick(bool trigger, QString name)
{
    qDebug() << name << " tri : " << trigger;
    auto func = actionEvntMap.find(name);
    if(func != actionEvntMap.end())
    {
        qDebug() << name << "  执行回调 ";
    }else {
        qDebug() << name << "  未注册回调方法 ";
    }
}

void MainWindow::initEvent()
{
    actionEvntMap = {
        {"new", std::bind(&MainWindow::newFile, this)},
        {"save", std::bind(&MainWindow::save, this)},
    };
}

bool MainWindow::newFile()
{
    if (maybeSave()) {
        isUntitled = true;
        curFile = tr("未命名.txt");
        setWindowTitle(curFile);
        ui->textEdit->clear();
        ui->textEdit->setVisible(true);

        return true;
    }
    return false;
}

bool MainWindow::maybeSave()
{
    // 如果文档被更改了
    if (ui->textEdit->document()->isModified()) {
    // 自定义一个警告对话框
       QMessageBox box;
       box.setWindowTitle(tr("警告"));
       box.setIcon(QMessageBox::Warning);
       box.setText(curFile + tr(" 尚未保存，是否保存？"));
       QPushButton *yesBtn = box.addButton(tr("是(&Y)"),
                        QMessageBox::YesRole);
       box.addButton(tr("否(&N)"), QMessageBox::NoRole);
       QPushButton *cancelBut = box.addButton(tr("取消"),
                        QMessageBox::RejectRole);
       box.exec();
       if (box.clickedButton() == yesBtn)
            return save();
       else if (box.clickedButton() == cancelBut)
            return false;
   }
   // 如果文档没有被更改，则直接返回true
   return true;
}

bool MainWindow::save()
{
   if (isUntitled) {
       return saveAs();
   } else {
       return saveFile(curFile);
   }
}

bool MainWindow::saveAs()
{
   QString fileName = QFileDialog::getSaveFileName(this, "另存为 ", curFile);
   if (fileName.isEmpty()) return false;
   return saveFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName)
{
   QFile file(fileName);

   if (!file.open(QFile::WriteOnly | QFile::Text)) {

       // %1和%2分别对应后面arg两个参数，/n起换行的作用
       QMessageBox::warning(this, tr("多文档编辑器"),
                            tr("无法写入文件 %1：/n %2")
                  .arg(fileName).arg(file.errorString()));
       return false;
   }
   QTextStream out(&file);
   // 鼠标指针变为等待状态
   QApplication::setOverrideCursor(Qt::WaitCursor);
   out << ui->textEdit->toPlainText();
   // 鼠标指针恢复原来的状态
   QApplication::restoreOverrideCursor();
   isUntitled = false;
   // 获得文件的标准路径
   curFile = QFileInfo(fileName).canonicalFilePath();
   setWindowTitle(curFile);
   return true;
}

