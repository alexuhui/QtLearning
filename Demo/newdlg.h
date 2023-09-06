#ifndef NEWDLG_H
#define NEWDLG_H

#include <QDialog>

namespace Ui {
class NewDlg;
}

class NewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewDlg(QWidget *parent = nullptr);
    ~NewDlg();

private:
    Ui::NewDlg *ui;
};

#endif // NEWDLG_H
