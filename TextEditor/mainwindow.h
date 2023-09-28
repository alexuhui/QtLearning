#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMenuBar* menubar;
    QToolBar* toolbar;
    QStatusBar* statusbar;

    void createMenu();
    void createMenuBtn(QJsonValue& menuConf);
    void createMenuActionBtn(QMenu* menu, QJsonValue& actionConf, bool& toolbarSeparator);

    void onActionClick(bool trigger, QString name);

private:
    bool newFile();   // 新建操作
    bool maybeSave(); // 判断是否需要保存
    bool save();      // 保存操作
    bool saveAs();    // 另存为操作
    bool saveFile(const QString &fileName); // 保存文件
    std::map<QString, std::function<bool()>> actionEvntMap;
    void initEvent();

private:
    // 为真表示文件没有保存过，为假表示文件已经被保存过了
    bool isUntitled;
    // 保存当前文件的路径
    QString curFile;
    // 初始化
    void init();
};
#endif // MAINWINDOW_H
