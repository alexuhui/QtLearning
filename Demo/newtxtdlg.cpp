#include "newtxtdlg.h"
#include "ui_newtxtdlg.h"

NewTxtDlg::NewTxtDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTxtDlg)
{
    ui->setupUi(this);
}

NewTxtDlg::~NewTxtDlg()
{
    delete ui;
}
