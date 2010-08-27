#ifndef VMAILDIALOG_H
#define VMAILDIALOG_H

#include <QtGui>

namespace Ui {
    class VMailDialog;
}

class VMailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VMailDialog(QWidget *parent = 0);
    ~VMailDialog();

private:
    Ui::VMailDialog *ui;
};

#endif // VMAILDIALOG_H
