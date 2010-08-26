#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "GVContactsTable.h"
#include "SMSDlg.h"

// Required for Symbian (QSystemTrayIcon)
#include "OsDependent.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    //! Emitted to complete a dial
    void dialCanFinish ();

public slots:
    void log (const QString &strText, int level = 10);
    void setStatus (const QString &strText, int timeout = 3000);

private slots:
    // All initializations happen here
    void on_btnContacts_clicked();
    void init ();

    void on_actionE_xit_triggered();
    //! The Singleton Application class invokes this function
    void messageReceived (const QString &message);
    //! Invoked when the user clicks Login
    void on_action_Login_triggered();
    //! Called when the web component completes login - with success or failure
    void loginCompleted (bool bOk, const QVariantList &arrParams);
    //! Invoked when the system ray is clicked
    void systray_activated (QSystemTrayIcon::ActivationReason reason);
    //! Invoked when a message box is closed. Purely a cleanup function.
    void msgBox_buttonClicked (QAbstractButton *button);

    //! Character clicked from the dialpad
    void charClicked (QChar ch);
    //! Delete clicked from dialpad
    void charDeleted ();

    //! Invoked after all contacts have been parsed
    void getContactsDone (bool bOk);

    //!Invoked when the dialer widget call button is clicked
    void dialNow (const QString &strTarget);

    //! Invoked when dialing has started
    void dialInProgress (const QString &strNumber);
    //! Invoked to perform a dial
    void dialAccessNumber (const QString  &strAccessNumber,
                           const QVariant &context        );

    //! Invoked on user request to call a number
    void callNumber (const QString &strNumber,
                     const QString &strNameLink = QString());
    //! Invoked on user request to send an SMS to an unknown number
    void textANumber (const QString &strNumber,
                      const QString &strNameLink = QString());

    //! Invoked when contact details have been obtained
    void callWithContactInfo (const GVContactInfo &info, bool bCallback = true);
    //! Invoked when work for getting contact details is done - success or fail.
    void contactsLinkWorkDone (bool bOk, const QVariantList &arrParams);

    //! Callback for the webpage when it gets a contacts info
    void sendTextToContact (const GVContactInfo &info, bool bCallback = true);
    //! Invoked when the webpage is done sending all contact info for an SMS
    void contactsLinkWorkDoneSMS (bool bOk, const QVariantList &arrParams);

    //! Invoked when the user finally clicks on the send SMS button
    void sendSMS (const QStringList &arrNumbers, const QString &strText);
    //! Invoked whenever the SMS has been sent
    void sendSMSDone (bool bOk, const QVariantList &arrParams);

private:
    void beginLogin ();
    void initContactsWidget ();
    void deinitContactsWidget ();
    bool getInfoFrom (const QString &strNumber,
                      const QString &strNameLink,
                      GVContactInfo &info);
    void closeEvent (QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    // Tray, icons, widgets
    QIcon           icoGoogle;
    QSystemTrayIcon *pSystray;
    //! Contacts table widget
    GVContactsTable *pContactsView;
    //! SMS Window
    SMSDlg          dlgSMS;

    //! Are we logged in?
    bool            bLoggedIn;
    //! User name
    QString         strUser;
    //! Password
    QString         strPass;
    //! Our own GV phone number
    QString         strSelfNumber;

    //! Set this flag if the user cancels the dialed number
    bool            bDialCancelled;
};

#endif // MAINWINDOW_H
