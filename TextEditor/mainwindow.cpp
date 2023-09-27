#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createMenuBtns();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenuBtns()
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

    // 读取配置项
    QJsonArray arr = doc.array();
    foreach (auto btn, arr) {
        QJsonValue obj = btn;
        QString name = obj["name"].toString();
        QString txt = obj["text"].toString();
        QString icon = obj["iocn"].toString();
        QString hotkey = obj["hotKey"].toString();
        createMenuBtn(name, txt, icon, hotkey);
    }
}

void MainWindow::createMenuBtn(QString& name, QString& txt, QString& iconPath, QString&  hotkey)
{
    // 创建新的动作
    QAction *openAction = new QAction(txt, this);
    openAction->setObjectName(name);
    // 添加图标
    QIcon icon(iconPath);
    openAction->setIcon(icon);
    // 设置快捷键
    openAction->setShortcut(QKeySequence(hotkey));
    // 在文件菜单中设置新的打开动作
    ui->menu_File->addAction(openAction);
}

