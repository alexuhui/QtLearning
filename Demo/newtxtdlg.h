#ifndef NEWTXTDLG_H
#define NEWTXTDLG_H

#include <QDialog>

namespace Ui {
class NewTxtDlg;
}

class NewTxtDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewTxtDlg(QWidget *parent = nullptr);
    ~NewTxtDlg();

private:
    Ui::NewTxtDlg *ui;
};

#endif // NEWTXTDLG_H
