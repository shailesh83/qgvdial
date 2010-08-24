#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "LoginDialog.h"
#include "Singletons.h"
#include "PhoneNumberValidator.h"

MainWindow::MainWindow (QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, icoGoogle(":/Google.png")
, pSystray(NULL)
{
    ui->setupUi(this);

    // Additional UI initializations:
    ui->btnDelete->setDelete (true);
    ui->edNumber->setValidator (new PhoneNumberValidator (ui->edNumber));

    // A systray icon if the OS supports it
    if (QSystemTrayIcon::isSystemTrayAvailable ())
    {
        pSystray = new QSystemTrayIcon (this);
        pSystray->setIcon (icoGoogle);
        pSystray->setToolTip ("Google Voice dialer");
        QObject::connect (
            pSystray,
            SIGNAL (activated (QSystemTrayIcon::ActivationReason)),
            this,
            SLOT (systray_activated (QSystemTrayIcon::ActivationReason)));
        pSystray->show ();
    }

    QObject::connect (qApp, SIGNAL (messageReceived (const QString &)),
                      this, SLOT   (messageReceived (const QString &)));
}//MainWindow::MainWindow

MainWindow::~MainWindow ()
{
    delete ui;
}//MainWindow::~MainWindow

void
MainWindow::setStatus(const QString &strText, int timeout /* = 0*/)
{
//    log (strText);
    ui->statusBar->showMessage (strText, timeout);
}//MainWindow::setStatus

void
MainWindow::messageReceived (const QString &message)
{
    if (message == "show") {
        this->show ();
    }
}//MainWindow::messageReceived

void
MainWindow::on_action_Login_triggered ()
{
    LoginDialog dlg (this);
    if (QDialog::Rejected == dlg.exec ()) {
        setStatus ("User cancelled login");
        return;
    }

    QString strUser, strPass;
    dlg.getUserPass (strUser, strPass);

    setStatus (QString ("User = %1, password = %2")
                .arg (strUser).arg (strPass));
}//MainWindow::on_action_Login_triggered

void
MainWindow::systray_activated (QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        if (this->isVisible ()) {
            this->hide ();
        } else {
            this->show ();
        }
        break;

    default:
        break;
    }
}//MainWindow::systray_activated

void
MainWindow::charClicked (QChar ch)
{
    ui->edNumber->insert (ch);
}//MainWindow::charClicked

void
MainWindow::charDeleted ()
{
    ui->edNumber->backspace ();
}//MainWindow::charDeleted

void
MainWindow::on_actionE_xit_triggered ()
{
    this->close ();
    qApp->quit ();
}//MainWindow::on_actionE_xit_triggered
