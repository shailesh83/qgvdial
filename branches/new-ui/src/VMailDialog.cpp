#include "VMailDialog.h"
#include "ui_VMailDialog.h"

VMailDialog::VMailDialog (QWidget *parent)
: QDialog(parent)
, ui(new Ui::VMailDialog)
{
    ui->setupUi(this);
}//VMailDialog::VMailDialog

VMailDialog::~VMailDialog()
{
    delete ui;
}//VMailDialog::~VMailDialog
