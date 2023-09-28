#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtextdocument.h"
#include "src/conf/confuser.h"
#include <QMainWindow>
#include <map>
#include <QCloseEvent>

class QLineEdit;
class QDialog;

QT_BEGIN_NAMESPACE namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMenuBar* menubar;
    QToolBar* toolbar;
    QStatusBar* statusbar;
    QLineEdit *findLineEdit;
    QDialog *findDlg;
    QTextDocument::FindFlags findFlags;

private:
    ConfUser confUser;

private:
    void createMenu();
    void createMenuBtn(QJsonValue& menuConf);
    void createMenuActionBtn(QMenu* menu, QJsonValue& actionConf, bool& toolbarSeparator);

    void onActionClick(bool trigger, QString name);

private:
    std::map<QString, std::function<bool()>> actionEvntMap;
    void initEvent();

private:
    // 为真表示文件没有保存过，为假表示文件已经被保存过了
    bool isUntitled;
    // 保存当前文件的路径
    QString curFile;
    // 初始化
    void init();

private:
    bool saveFile(const QString &fileName); // 保存文件
    bool loadFile(const QString &fileName); // 加载文件
    bool maybeSave(); // 判断是否需要保存
    bool newFile();   // 新建操作
    bool openFile();  // 打开文件
    bool closeFile(); // 关闭文件
    bool save();      // 保存操作
    bool saveAs();    // 另存为操作
    bool exit();      // 退出
    bool undo();      // 撤销
    bool cut();       // 剪切
    bool copy();      // 复制
    bool paste();     // 粘贴
    bool findTxt();   // 搜索


private slots:
    void showFindText();

private:
    void findInit();
    void updateFindTextFlags(QTextDocument::FindFlag flag, bool apply = true);
};

#endif // MAINWINDOW_H
