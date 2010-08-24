#ifndef DIALDIGITBUTTON_H
#define DIALDIGITBUTTON_H

#include <QPushButton>

class DialDigitButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DialDigitButton (QWidget *parent = 0);
    void setDelete (bool bFlag = true);

signals:
    void charClicked (QChar ch);
    void charDeleted ();

private slots:
    void _i_clicked ();

private:
    bool bDelete;
};

#endif // DIALDIGITBUTTON_H
