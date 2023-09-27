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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
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

}

