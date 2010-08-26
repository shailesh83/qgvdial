#include <QtCore>
#include "LoginDialog.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog (QWidget *parent)
: QDialog(parent)
, ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}//LoginDialog::LoginDialog

LoginDialog::~LoginDialog ()
{
    delete ui;
}//LoginDialog::~LoginDialog

bool
LoginDialog::getUserPass (QString &strUser, QString &strPass)
{
    if (QDialog::Accepted != this->result ()) {
        return (false);
    }

    strUser = ui->edUsername->text ();
    strPass = ui->edPassword->text ();

    return (true);
}//LoginDialog::getUserPass
