#ifndef TXTDLG_H
#define TXTDLG_H

#include <QDialog>

namespace Ui {
class TxtDlg;
}

class TxtDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TxtDlg(QWidget *parent = nullptr);
    ~TxtDlg();

private:
    Ui::TxtDlg *ui;
};

#endif // TXTDLG_H
