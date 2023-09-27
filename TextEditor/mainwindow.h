#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};
#endif // MAINWINDOW_H
