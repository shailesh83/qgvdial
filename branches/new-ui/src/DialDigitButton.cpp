#include "DialDigitButton.h"

DialDigitButton::DialDigitButton (QWidget *parent)
: QPushButton (parent)
, bDelete (false)
{
    QObject::connect (this, SIGNAL (clicked ()),
                      this, SLOT   (_i_clicked ()));
}//DialDigitButton::DialDigitButton

void
DialDigitButton::setDelete (bool bFlag /*= true*/)
{
    bDelete = bFlag;
}//DialDigitButton::setDelete

void
DialDigitButton::_i_clicked ()
{
    if (!bDelete) {
        emit charClicked (this->text()[0]);
    } else {
        emit charDeleted ();
    }
}//DialDigitButton::_i_clicked
