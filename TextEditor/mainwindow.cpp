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
#include <QLineEdit>
#include <QDialog>
#include <QHBoxLayout>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMenu();
    init();
    findInit();
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

void MainWindow::findInit()
{
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("查找"));
    findLineEdit = new QLineEdit(findDlg);
    // 整体垂直布局
    QVBoxLayout *vLayout= new QVBoxLayout(findDlg);
    vLayout->addWidget(findLineEdit);

    // 查找规则水平布局
    QHBoxLayout *hLayout = new QHBoxLayout(findDlg);
    vLayout->addLayout(hLayout);

    QCheckBox *qcbFindBack = new QCheckBox(tr("反向查找"), findDlg);
    hLayout->addWidget(qcbFindBack);
    connect(qcbFindBack, &QCheckBox::clicked, this, [this](bool checked){
        updateFindTextFlags(QTextDocument::FindBackward, checked);
    });
    qcbFindBack->setCheckState(Qt::CheckState::Unchecked);

    QCheckBox *qcbIgnoreCase = new QCheckBox(tr("大小写敏感"), findDlg);
    hLayout->addWidget(qcbIgnoreCase);
    connect(qcbIgnoreCase, &QCheckBox::clicked, this, [this](bool checked){
        updateFindTextFlags(QTextDocument::FindCaseSensitively, checked);
    });
    qcbIgnoreCase->setCheckState(Qt::CheckState::Unchecked);

    QCheckBox *qcbWholeWords = new QCheckBox(tr("全词匹配"), findDlg);
    hLayout->addWidget(qcbWholeWords);
    connect(qcbWholeWords, &QCheckBox::clicked, this, [this](bool checked){
        updateFindTextFlags(QTextDocument::FindWholeWords, checked);
    });
    qcbWholeWords->setCheckState(Qt::CheckState::Unchecked);

    QPushButton *btn= new QPushButton(tr("查找下一个"), findDlg);
    vLayout->addWidget(btn);
    connect(btn, SIGNAL(clicked()), this, SLOT(showFindText()));
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
//    qDebug() << name << " tri : " << trigger;
    auto kv = actionEvntMap.find(name);
    if(kv != actionEvntMap.end())
    {
        auto func = kv->second;
        if(func != NULL)
        {
            func();
        }else{
            qDebug() << name << "  回调方法未实现 ";
        }

    }else {
        qDebug() << name << "  未注册回调方法 ";
    }
}

void MainWindow::showFindText()
{
    QString str = findLineEdit->text();
    bool result = ui->textEdit->find(str, findFlags);
//    qDebug() << findFlags ;
    if(!result)
    {
        // 尝试跳到头/尾重查一次
        if((findFlags & QTextDocument::FindBackward) == 0)
        {
            //正向搜索，重置光标到文本开始
             ui->textEdit->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
        }else
        {
            //反向搜索，重置光标到文本末尾
            ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        }
        result = ui->textEdit->find(str, findFlags);
    }

    if (!result)
    {
       QMessageBox::warning(this, tr("查找"),
                tr("找不到%1").arg(str));
    }
}

void MainWindow::updateFindTextFlags(QTextDocument::FindFlag flag, bool apply)
{
//    qDebug() << "updateFindTextFlags before :" << findFlags << "  " << apply;
    if(apply)
    {
       findFlags |= flag;
    }else {
       findFlags &= ~flag;
    }
//    qDebug() << "updateFindTextFlags after :" << findFlags;
}

void MainWindow::initEvent()
{
    actionEvntMap = {
        {"new", std::bind(&MainWindow::newFile, this)},
        {"open", std::bind(&MainWindow::openFile, this)},
        {"close", std::bind(&MainWindow::close, this)},
        {"save", std::bind(&MainWindow::save, this)},
        {"save_as", std::bind(&MainWindow::saveAs, this)},
        {"exit", std::bind(&MainWindow::exit, this)},
        {"undo", std::bind(&MainWindow::undo, this)},
        {"cut", std::bind(&MainWindow::cut, this)},
        {"copy", std::bind(&MainWindow::copy, this)},
        {"paste", std::bind(&MainWindow::paste, this)},
        {"find", std::bind(&MainWindow::findTxt, this)},
        {"version", NULL},

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

bool MainWindow::openFile()
{
    if (maybeSave()) {

        QString fileName = QFileDialog::getOpenFileName(this);

        // 如果文件名不为空，则加载文件
        if (!fileName.isEmpty()) {
            loadFile(fileName);
            ui->textEdit->setVisible(true);
        }

        return true;
    }
    return false;
}

bool MainWindow::closeFile()
{
    if (maybeSave()) {
        ui->textEdit->setVisible(false);
        return true;
    }
    return false;
}

bool MainWindow::exit()
{
    if(!closeFile())
    {
        return false;
    }
    qApp->quit();
    return true;
}

bool MainWindow::undo()
{
    ui->textEdit->undo();
    return true;
}

bool MainWindow::cut()
{
    ui->textEdit->cut();
    return true;
}

bool MainWindow::copy()
{
    ui->textEdit->copy();
    return true;
}

bool MainWindow::paste()
{
    ui->textEdit->paste();
    return true;
}

bool MainWindow::findTxt()
{
    findDlg->show();
    return true;
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

bool MainWindow::loadFile(const QString &fileName)
{
   QFile file(fileName); // 新建QFile对象
   if (!file.open(QFile::ReadOnly | QFile::Text)) {
       QMessageBox::warning(this, tr("多文档编辑器"),
                            tr("无法读取文件 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
       return false; // 只读方式打开文件，出错则提示，并返回false
   }
   QTextStream in(&file); // 新建文本流对象
   QApplication::setOverrideCursor(Qt::WaitCursor);
   // 读取文件的全部文本内容，并添加到编辑器中
   ui->textEdit->setPlainText(in.readAll());      QApplication::restoreOverrideCursor();

   // 设置当前文件
   curFile = QFileInfo(fileName).canonicalFilePath();
   setWindowTitle(curFile);
   return true;
}

