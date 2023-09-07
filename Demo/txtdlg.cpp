#include "txtdlg.h"
#include "ui_txtdlg.h"

TxtDlg::TxtDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TxtDlg)
{
    ui->setupUi(this);
}

TxtDlg::~TxtDlg()
{
    delete ui;
}
