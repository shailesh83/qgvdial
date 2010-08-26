#include "global.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "LoginDialog.h"
#include "DialCancelDlg.h"
#include "DlgSelectContactNumber.h"

#include "Singletons.h"
#include "PhoneNumberValidator.h"

#include <iostream>
using namespace std;

MainWindow::MainWindow (QWidget *parent)
: QMainWindow (parent)
, ui (new Ui::MainWindow)
, icoGoogle (":/Google.png")
, pSystray (NULL)
, pContactsView (NULL)
, bLoggedIn (false)
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

    QTimer::singleShot (1000, this, SLOT (init()));
}//MainWindow::MainWindow

MainWindow::~MainWindow ()
{
    delete ui;
}//MainWindow::~MainWindow

void
MainWindow::log (const QString &strText, int level /*= 10*/)
{
    QDateTime dt = QDateTime::currentDateTime ();
    QString strLog = QString("%1 : %2 : %3")
                     .arg(dt.toString ("yyyy-MM-dd hh:mm:ss.zzz"))
                     .arg(level)
                     .arg(strText);

    // Send to plain text widget
//    if (NULL != txtLogs) {
//        txtLogs->appendPlainText (strLog);
//    }

    // Send to standard output
    cout << strLog.toStdString () << endl;

    // Send to log file
//    if (fLogfile.isOpen ()) {
//        QTextStream streamLog(&fLogfile);
//        streamLog << strLog << endl;
//    }
}//MainWindow::log

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
MainWindow::init ()
{
    GVAccess &webPage = Singletons::getRef().getGVAccess ();
    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    OsDependent &osd = Singletons::getRef().getOSD ();

    setStatus ("Initializing");

    dbMain.init ();
    osd.initDialServer (this, SLOT (dialNow (const QString &)));

    // Dialing handshake
    QObject::connect (&webPage    , SIGNAL (dialInProgress (const QString &)),
                       this       , SLOT   (dialInProgress (const QString &)));
    QObject::connect ( this       , SIGNAL (dialCanFinish ()),
                      &webPage    , SLOT   (dialCanFinish ()));
    QObject::connect (
        &webPage, SIGNAL (dialAccessNumber (const QString &,
                                            const QVariant &)),
         this   , SLOT   (dialAccessNumber (const QString &,
                                            const QVariant &)));

    // Skype client factory log and status
    SkypeClientFactory &skypeFactory = Singletons::getRef().getSkypeFactory ();
    skypeFactory.setMainWidget (this);
    QObject::connect (
        &skypeFactory, SIGNAL (log(const QString &, int)),
         this        , SLOT   (log(const QString &, int)));
    QObject::connect (
        &skypeFactory, SIGNAL (status(const QString &, int)),
         this        , SLOT   (setStatus(const QString &, int)));

    // Observer factory log and status
    ObserverFactory &obF = Singletons::getRef().getObserverFactory ();
    obF.init ();
    QObject::connect (&obF , SIGNAL (log(const QString &, int)),
                       this, SLOT   (log(const QString &, int)));
    QObject::connect (&obF , SIGNAL (status(const QString &, int)),
                       this, SLOT   (setStatus(const QString &, int)));

    // webPage log and status
    QObject::connect (&webPage, SIGNAL (log(const QString &, int)),
                       this   , SLOT   (log(const QString &, int)));
    QObject::connect (&webPage, SIGNAL (status(const QString &, int)),
                       this   , SLOT   (setStatus(const QString &, int)));

    // call initiator log status and init
    CallInitiatorFactory& cif = Singletons::getRef().getCIFactory ();
    QObject::connect (&cif , SIGNAL (log(const QString &, int)),
                       this, SLOT   (log(const QString &, int)));
    QObject::connect (&cif , SIGNAL (status(const QString &, int)),
                       this, SLOT   (setStatus(const QString &, int)));

    // If the cache has the username and password, begin login
    if (dbMain.getUserPass (strUser, strPass))
    {
        beginLogin ();
    }
    else
    {
        setStatus ("No user credentials cached. Please log in");

        strUser.clear ();
        strPass.clear ();
    }

}//MainWindow::init

void
MainWindow::beginLogin ()
{
    GVAccess &webPage = Singletons::getRef().getGVAccess ();

    bool bOk = false;
    do // Begin cleanup block (not a loop)
    {
//@@UV: Uncomment
//        pContactsView->setUserPass (strUser, strPass);

        QVariantList l;
        l += strUser;
        l += strPass;

        setStatus ("Beginning login");
        // webPage.workCompleted -> this.loginCompleted
        if (!webPage.enqueueWork (GVAW_login, l, this,
                SLOT (loginCompleted (bool, const QVariantList &))))
        {
            log ("Login returned immediately with failure!", 3);
            break;
        }

        bOk = true;
    } while (0); // End cleanup block (not a loop)

    if (!bOk)
    {
        // Cleanup if any
        strUser.clear ();
        strPass.clear ();

//@@UV: Uncomment
//        enterNotLoggedIn ();
    }
}//MainWindow::beginLogin

void
MainWindow::on_action_Login_triggered ()
{
    do // Begin cleanup block (not a loop)
    {
        if (!bLoggedIn) {
            LoginDialog dlg (this);
            if (QDialog::Rejected == dlg.exec ()) {
                setStatus ("User cancelled login");
                break;
            }
            if (!dlg.getUserPass (strUser, strPass)) {
                setStatus ("Invalid username or password");
                break;
            }

            beginLogin ();
        } else {
            //@@UV: Logout
        }
    } while (0); // End cleanup block (not a loop)
}//MainWindow::on_action_Login_triggered

void
MainWindow::loginCompleted (bool bOk, const QVariantList &varList)
{
    strSelfNumber.clear ();

    if (!bOk)
    {
        setStatus ("User login failed");

        // Cleanup if any
        QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,
                           "Invalid username or password",
                           "Username or password not recognized",
                           QMessageBox::Close,
                           this);
        msgBox->setModal (false);
        QObject::connect (
            msgBox, SIGNAL (buttonClicked (QAbstractButton *)),
            this  , SLOT   (msgBox_buttonClicked (QAbstractButton *)));
        msgBox->show ();

        ui->action_Login->setText ("Login...");
    }
    else
    {
        setStatus ("User logged in");

        strSelfNumber = varList[varList.size()-1].toString ();

        initContactsWidget ();

        ui->action_Login->setText ("Logout");
        ui->btnContacts->setEnabled (true);
    }
}//MainWindow::loginCompleted

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
MainWindow::msgBox_buttonClicked (QAbstractButton *button)
{
    if (NULL != button->parent ())
    {
        button->parent()->deleteLater ();
    }
}//MainWindow::msgBox_buttonClicked

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

void
MainWindow::getContactsDone (bool bOk)
{
    if (!bOk)
    {
        QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,
                           "Error",
                           "Contacts retrieval failed",
                           QMessageBox::Close);
        msgBox->setModal (false);
        QObject::connect (
            msgBox, SIGNAL (buttonClicked (QAbstractButton *)),
            this  , SLOT   (msgBox_buttonClicked (QAbstractButton *)));
        msgBox->show ();
        setStatus ("Contacts retrieval failed");
    }
}//MainWindow::getContactsDone

void
MainWindow::initContactsWidget ()
{
    do { // Begin cleanup block (not a loop)
        if (NULL != pContactsView) {
            log ("Contacts widget it already active");
            break;
        }

        // Create the contact view
        pContactsView = new GVContactsTable ();

        // Log and status
        QObject::connect (
            pContactsView, SIGNAL (log(const QString &, int)),
            this         , SLOT   (log(const QString &, int)));
        QObject::connect (
            pContactsView, SIGNAL (status   (const QString &, int)),
            this         , SLOT   (setStatus(const QString &, int)));

        // pContactsView.allContacts -> this.getContactsDone
        QObject::connect (pContactsView, SIGNAL (allContacts (bool)),
                          this         , SLOT   (getContactsDone (bool)));
        // pContactsView.call -> this.call
        QObject::connect (
            pContactsView,
                SIGNAL(callNumber(const QString &, const QString &)),
            this         ,
                SLOT  (callNumber(const QString &, const QString &)));
        // pContactsView.SMS -> this.SMS
        QObject::connect (
            pContactsView,
                SIGNAL (textANumber (const QString &, const QString &)),
            this         ,
                SLOT   (textANumber (const QString &, const QString &)));

        pContactsView->setUserPass (strUser, strPass);
        pContactsView->loginSuccess ();
        pContactsView->initModel ();
    } while (0); // End cleanup block (not a loop)
}//MainWindow::initContactsWidget

void
MainWindow::deinitContactsWidget ()
{
    do { // Begin cleanup block (not a loop)
        if (NULL == pContactsView) {
            log ("Contacts widget was NULL.");
            break;
        }

        pContactsView->loggedOut ();

        pContactsView->deleteLater ();
        pContactsView = NULL;
    } while (0); // End cleanup block (not a loop)
}//MainWindow::deinitContactsWidget

bool
MainWindow::getInfoFrom (const QString &strNumber,
                         const QString &strNameLink,
                         GVContactInfo &info)
{
    info = GVContactInfo();

    if (0 != strNameLink.size ())
    {
        CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
        info.strLink = strNameLink;

        if (!dbMain.getContactFromLink (info))
        {
            QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,
                                        "Get Contact failure",
                                        "Failed to get contact information",
                                        QMessageBox::Close,
                                        this);
            msgBox->setModal (false);
            QObject::connect (
                msgBox, SIGNAL (buttonClicked (QAbstractButton *)),
                this  , SLOT   (msgBox_buttonClicked (QAbstractButton *)));
            msgBox->show ();

            return (false);
        }

        for (int i = 0; i < info.arrPhones.size (); i++)
        {
            QString lhs = strNumber;
            QString rhs = info.arrPhones[i].strNumber;

            GVAccess::simplify_number (lhs);
            GVAccess::simplify_number (rhs);
            if (lhs == rhs)
            {
                info.selected = i;
                break;
            }
        }
    }
    else
    {
        info.strName = strNumber;
        GVContactNumber num;
        num.chType = 'O';
        num.strNumber = strNumber;
        info.arrPhones += num;
        info.selected = 0;
    }

    return (true);
}//MainWindow::getInfoFrom

void
MainWindow::callNumber (const QString &strNumber,
                        const QString &strNameLink)
{
    GVContactInfo info;

    if (!getInfoFrom (strNumber, strNameLink, info))
    {
        return;
    }

    callWithContactInfo (info, false);
}//MainWindow::callNumber

void
MainWindow::textANumber (const QString &strNumber,
                         const QString &strNameLink)
{
    GVContactInfo info;

    if (!getInfoFrom (strNumber, strNameLink, info))
    {
        return;
    }

    sendTextToContact (info, false);
}//MainWindow::textANumber

void
MainWindow::callWithContactInfo (const GVContactInfo &info, bool bSaveIt)
{
    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    if (bSaveIt)
    {
        dbMain.putContactInfo (info);
    }
    else
    {
        log ("Got contact info from cached location");
    }

    DlgSelectContactNumber dlg(info, this);
    int rv = dlg.exec ();
    do // Begin cleanup block (not a loop)
    {
        if (QDialog::Accepted != rv)
        {
            log ("User canceled call", 3);
            break;
        }
        rv = dlg.getSelection ();
        if (-1 == rv)
        {
            log ("Invalid selection", 3);
            break;
        }

        dialNow (info.arrPhones[rv].strNumber);
    } while (0); // End cleanup block (not a loop)
}//MainWindow::callWithContactInfo

void
MainWindow::contactsLinkWorkDone (bool, const QVariantList &)
{
    GVAccess &webPage = Singletons::getRef().getGVAccess ();
    QObject::disconnect (
        &webPage, SIGNAL (contactInfo (const GVContactInfo &)),
         this   , SLOT   (callWithContactInfo (const GVContactInfo &)));

    setStatus ("Retrieved contact info");
}//MainWindow::contactsLinkWorkDone

void
MainWindow::dialNow (const QString &strTarget)
{
    bool bDialout;
    CalloutInitiator *ci;

    do // Begin cleanup block (not a loop)
    {
        GVRegisteredNumber gvRegNumber;
        //@@UV: Uncomment
//        if (!pGVSettings->getDialSettings (bDialout, gvRegNumber, ci))
//        {
//            setStatus ("Unable to dial out because settings are not valid");
//            break;
//        }

        GVAccess &webPage = Singletons::getRef().getGVAccess ();
        QVariantList l;
        l += strTarget;     // The destination number is common between the two

        if (bDialout)
        {
            l += ci->selfNumber ();
            l += QVariant::fromValue<void*>(ci) ;
            if (!webPage.enqueueWork (GVAW_dialOut, l, this,
                            SLOT (dialComplete (bool, const QVariantList &))))
            {
                setStatus ("Dialing failed instantly");
            }
        }
        else
        {
            l += gvRegNumber.strNumber;
            l += QString (gvRegNumber.chType);
            if (!webPage.enqueueWork (GVAW_dialCallback, l, this,
                            SLOT (dialComplete (bool, const QVariantList &))))
            {
                setStatus ("Dialing failed instantly");
            }
        }
    } while (0); // End cleanup block (not a loop)
}//MainWindow::dialNow

void
MainWindow::dialInProgress (const QString &strNumber)
{
    bDialCancelled = false;

    DialCancelDlg msgBox(strNumber, this);
    int ret = msgBox.doModal (strSelfNumber);
    if (QMessageBox::Ok == ret)
    {
        emit dialCanFinish ();
    }
    else
    {
        bDialCancelled = true;
        GVAccess &webPage = Singletons::getRef().getGVAccess ();
        webPage.cancelWork (GVAW_dialCallback);
    }
}//MainWindow::dialInProgress

void
MainWindow::dialAccessNumber (const QString  &strAccessNumber,
                              const QVariant &context        )
{
    CalloutInitiator *ci = (CalloutInitiator *) context.value<void *>();
    if (NULL != ci)
    {
        ci->initiateCall (strAccessNumber);
        setStatus ("Callout in progress");
    }
    else
    {
        log ("Invalid call out initiator", 3);
        setStatus ("Callout failed");
    }
}//MainWindow::dialAccessNumber

void
MainWindow::sendTextToContact (const GVContactInfo &info, bool bSaveIt)
{
    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    if (bSaveIt)
    {
        dbMain.putContactInfo (info);
    }
    else
    {
        log ("Got contact info from cached location");
    }

    DlgSelectContactNumber dlg(info, this);
    int rv = dlg.exec ();
    do // Begin cleanup block (not a loop)
    {
        if (QDialog::Accepted != rv)
        {
            log ("User canceled SMS", 3);
            break;
        }
        rv = dlg.getSelection ();
        if (-1 == rv)
        {
            log ("Invalid selection", 3);
            break;
        }

        SMSEntry entry;
        entry.strName = info.strName;
        entry.sNumber = info.arrPhones[rv];

        dlgSMS.addSMSEntry (entry);

        if (dlgSMS.isHidden ())
        {
            dlgSMS.show ();
        }
    } while (0); // End cleanup block (not a loop)
}//MainWindow::sendTextToContact

void
MainWindow::contactsLinkWorkDoneSMS (bool, const QVariantList &)
{
    GVAccess &webPage = Singletons::getRef().getGVAccess ();
    QObject::disconnect (
        &webPage, SIGNAL (contactInfo       (const GVContactInfo &)),
         this   , SLOT   (sendTextToContact (const GVContactInfo &)));

    setStatus ("Retrieved contact info");
}//MainWindow::contactsLinkWorkDoneSMS

void
MainWindow::sendSMS (const QStringList &arrNumbers, const QString &strText)
{
    GVAccess &webPage = Singletons::getRef().getGVAccess ();
    QStringList arrFailed;

    for (int i = 0; i < arrNumbers.size (); i++)
    {
        QVariantList l;
        l += arrNumbers[i];
        l += strText;
        if (!webPage.enqueueWork (GVAW_sendSMS, l, this,
                SLOT (sendSMSDone (bool, const QVariantList &))))
        {
            arrFailed += arrNumbers[i];
            QString msg = QString ("Failed to send an SMS to %1")
                                   .arg (arrNumbers[i]);
            log (msg, 3);
            break;
        }
    } // loop through all the numbers

    if (0 != arrFailed.size ())
    {
        QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,
                                    "SMS could not be sent to the following:",
                                    arrFailed.join (", "),
                                    QMessageBox::Close,
                                    this);
        msgBox->setModal (false);
        QObject::connect (
            msgBox, SIGNAL (buttonClicked (QAbstractButton *)),
            this  , SLOT   (msgBox_buttonClicked (QAbstractButton *)));
        msgBox->show ();
    }
}//MainWindow::sendSMS

void
MainWindow::sendSMSDone (bool bOk, const QVariantList &)
{
    if (!bOk)
    {
        setStatus ("Failed to send SMS");
    }
    else
    {
        setStatus ("SMS sent");
    }
}//MainWindow::sendSMSDone

void
MainWindow::on_btnContacts_clicked ()
{
    initContactsWidget ();
    pContactsView->show ();
}//MainWindow::on_btnContacts_clicked
