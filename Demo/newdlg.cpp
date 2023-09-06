#include "newdlg.h"
#include "ui_newdlg.h"

NewDlg::NewDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDlg)
{
    ui->setupUi(this);
}

NewDlg::~NewDlg()
{
    delete ui;
}
