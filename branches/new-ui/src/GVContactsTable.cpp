#include "global.h"
#include "GVContactsTable.h"
#include "ui_ContactsWidget.h"
#include "CaptchaWidget.h"

#include "Singletons.h"
#include "ContactsXmlHandler.h"

GVContactsTable::GVContactsTable (QWidget *parent, Qt::WindowFlags flags)
: QMainWindow (parent, flags)
, ui (new Ui::ContactsWindow)
, nwMgr (this)
, mutex(QMutex::Recursive)
, bLoggedIn(false)
, bRefreshRequested (false)
{
    ui->setupUi (this);

#ifdef Q_WS_MAEMO_5
    this->setAttribute (Qt::WA_Maemo5StackedWindow);
#endif

    mnuContext.addAction (ui->actionCall);
    mnuContext.addAction (ui->actionSend_Text);

    // treeView.activated -> this.activatedContact
    QObject::connect (
        ui->treeView, SIGNAL (activated        (const QModelIndex &)),
        this        , SLOT   (activatedContact (const QModelIndex &)));

    // The status must be shown on this window as well
    QObject::connect (
        this, SIGNAL (status    (const QString &, int)),
        this, SLOT   (setStatus (const QString &, int)));
}//GVContactsTable::GVContactsTable

GVContactsTable::~GVContactsTable ()
{
    deinitModel ();
}//GVContactsTable::~GVContactsTable

void
GVContactsTable::deinitModel ()
{
    ui->treeView->reset ();

    QSqlTableModel *modelContacts = (QSqlTableModel *)ui->treeView->model ();
    ui->treeView->setModel (NULL);
    if (NULL != modelContacts)
    {
        delete modelContacts;
        modelContacts = NULL;
    }
}//GVContactsTable::deinitModel

void
GVContactsTable::initModel ()
{
    deinitModel ();

    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    QSqlTableModel *modelContacts = dbMain.newContactsModel ();
    ui->treeView->setModel (modelContacts);
    modelContacts->submitAll ();

    ui->treeView->hideColumn (1);
    ui->treeView->sortByColumn (0, Qt::AscendingOrder);
}//GVContactsTable::initModel

QNetworkReply *
GVContactsTable::postRequest (QString         strUrl,
                              QStringPairList arrPairs,
                              QObject        *receiver,
                              const char     *method)
{
    QStringList arrParams;
    foreach (QStringPair pairParam, arrPairs)
    {
        arrParams += QString("%1=%2")
                        .arg(pairParam.first)
                        .arg(pairParam.second);
    }
    QString strParams = arrParams.join ("&");

    QUrl url (strUrl);
    QNetworkRequest request(url);
    request.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");
    if (0 != strGoogleAuth.size ())
    {
        QByteArray byAuth = QString("GoogleLogin auth=%1")
                                    .arg(strGoogleAuth).toAscii ();
        request.setRawHeader ("Authorization", byAuth);
    }
    QByteArray byPostData = strParams.toAscii ();

    QObject::connect (&nwMgr   , SIGNAL (finished (QNetworkReply *)),
                       receiver, method);
    QNetworkReply *reply = nwMgr.post (request, byPostData);
    return (reply);
}//GVContactsTable::postRequest

QNetworkReply *
GVContactsTable::getRequest (QString         strUrl,
                             QObject        *receiver,
                             const char     *method)
{
    QUrl url (strUrl);
    QNetworkRequest request(url);
    request.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");
    if (0 != strGoogleAuth.size ())
    {
        QByteArray byAuth = QString("GoogleLogin auth=%1")
                                    .arg(strGoogleAuth).toAscii ();
        request.setRawHeader ("Authorization", byAuth);
    }

    QObject::connect (&nwMgr   , SIGNAL (finished (QNetworkReply *)),
                       receiver, method);
    QNetworkReply *reply = nwMgr.get (request);
    return (reply);
}//GVContactsTable::getRequest

void
GVContactsTable::refreshContacts ()
{
    QMutexLocker locker(&mutex);
    if (!bLoggedIn)
    {
        bRefreshRequested = true;
        return;
    }
    bRefreshRequested = false;

    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    QString strUrl;

    strSavedLink.clear ();
    strUrl = QString ("http://www.google.com/m8/feeds/contacts/%1/full"
                      "?max-results=10000")
                        .arg (strUser);

    bRefreshIsUpdate = false;
    QDateTime dtUpdate;
    if ((dbMain.getLastContactUpdate (dtUpdate)) && (dtUpdate.isValid ()))
    {
        QString strUpdate = dtUpdate.toString ("yyyy-MM-ddThh:mm:ss");
        strUrl += QString ("&updated-min=%1&showdeleted=true").arg (strUpdate);
        bRefreshIsUpdate = true;
    }
    else
    {
        dbMain.clearContacts ();
    }

    emit status ("Retrieving contacts", 0);
    getRequest (strUrl, this , SLOT (onGotContacts (QNetworkReply *)));
}//GVContactsTable::refreshContactsFromContactsAPI

void
GVContactsTable::loginSuccess ()
{
    QMutexLocker locker(&mutex);

    QStringPairList arrPairs;
    arrPairs += QStringPair("accountType", "GOOGLE");
    arrPairs += QStringPair("Email"      , strUser);
    arrPairs += QStringPair("Passwd"     , strPass);
    arrPairs += QStringPair("service"    , "cp"); // name for contacts service
    arrPairs += QStringPair("source"     , "MyCompany-qgvdial-ver01");
    postRequest (GV_CLIENTLOGIN, arrPairs,
                 this , SLOT (onLoginResponse (QNetworkReply *)));
}//GVContactsTable::loginSuccess

void
GVContactsTable::loggedOut ()
{
    QMutexLocker locker(&mutex);
    bLoggedIn = false;

    strGoogleAuth.clear ();
}//GVContactsTable::loggedOut

void
GVContactsTable::activatedContact (const QModelIndex &)
{
    placeCall ();
}//GVContactsTable::activatedContact

void
GVContactsTable::selectionChanged (const QItemSelection &selected,
                                   const QItemSelection &/*deselected*/)
{
    QModelIndexList listModels = selected.indexes ();
    if (0 == listModels.size ())
    {
        strSavedLink.clear ();
        return;
    }
    QModelIndex linkIndex = listModels[0].model()->index (listModels[0].row(),
                                                          1);
    strSavedLink = linkIndex.data(Qt::EditRole).toString();
    if (strSavedLink.isEmpty ())
    {
        emit log ("Failed to get contact information", 3);
    }
}//GVContactsTable::selectionChanged

void
GVContactsTable::contextMenuEvent (QContextMenuEvent * event)
{
    mnuContext.popup (event->globalPos ());
}//GVContactsTable::contextMenuEvent

void
GVContactsTable::placeCall ()
{
    QMutexLocker locker(&mutex);
    if (0 != strSavedLink.size ())
    {
        emit callNumber (QString (), strSavedLink);
    }
    else
    {
        emit status ("Nothing selected");
    }
}//GVContactsTable::placeCall

void
GVContactsTable::sendSMS ()
{
    QMutexLocker locker(&mutex);
    if (0 != strSavedLink.size ())
    {
        emit textANumber (QString(), strSavedLink);
    }
    else
    {
        emit status ("Nothing selected");
    }
}//GVContactsTable::sendSMS

void
GVContactsTable::setUserPass (const QString &strU, const QString &strP)
{
    QMutexLocker locker(&mutex);
    strUser = strU;
    strPass = strP;
}//GVContactsTable::setUserPass

void
GVContactsTable::onLoginResponse (QNetworkReply *reply)
{
    QObject::disconnect (&nwMgr, SIGNAL (finished (QNetworkReply *)),
                          this , SLOT   (onLoginResponse (QNetworkReply *)));

    QString msg;
    QString strReply = reply->readAll ();
    QString strCaptchaToken, strCaptchaUrl;

    strGoogleAuth.clear ();
    do // Begin cleanup block (not a loop)
    {
        QStringList arrParsed = strReply.split ('\n');
        foreach (QString strPair, arrParsed)
        {
            QStringList arrPair = strPair.split ('=');
            if (arrPair[0] == "Auth")
            {
                strGoogleAuth = arrPair[1];
            }
            else if (arrPair[0] == "CaptchaToken")
            {
                strCaptchaToken = arrPair[1];
            }
            else if (arrPair[0] == "CaptchaUrl")
            {
                strCaptchaUrl = arrPair[1];
            }
        }

        if (0 != strCaptchaUrl.size ())
        {
            strCaptchaUrl = "http://www.google.com/accounts/"
                          + strCaptchaUrl;
            emit log ("Loading captcha");
            CaptchaWidget *captcha = new CaptchaWidget(strCaptchaUrl, this);
            QObject::connect (
                captcha, SIGNAL (done (bool, const QString &)),
                this   , SLOT   (onCaptchaDone (bool, const QString &)));
            break;
        }

        if (0 == strGoogleAuth.size ())
        {
            emit log ("Failed to login!!");
            break;
        }

        QMutexLocker locker (&mutex);
        bLoggedIn = true;

        emit log ("Login success");

        if (bRefreshRequested)
        {
            refreshContacts ();
        }
    } while (0); // End cleanup block (not a loop)
    reply->deleteLater ();
}//GVContactsTable::onLoginResponse

void
GVContactsTable::onCaptchaDone (bool bOk, const QString & /*strCaptcha*/)
{
    // No point disconnecting anything because the widget is going to delete
    // itself anyway.

    do { // Begin cleanup block (not a loop)
        if (!bOk)
        {
            log ("Captcha failed");
            break;
        }

        QStringPairList arrPairs;
        arrPairs += QStringPair("accountType", "GOOGLE");
        arrPairs += QStringPair("Email"      , strUser);
        arrPairs += QStringPair("Passwd"     , strPass);
        arrPairs += QStringPair("service"    , "grandcentral");
        arrPairs += QStringPair("source"     , "MyCompany-testapp16-ver01");
        //TODO: add captcha params
        postRequest (GV_CLIENTLOGIN, arrPairs,
                     this , SLOT (onLoginResponse (QNetworkReply *)));
    } while (0); // End cleanup block (not a loop)
}//GVContactsTable::onCaptchaDone

void
GVContactsTable::onGotContacts (QNetworkReply *reply)
{
    QObject::disconnect (&nwMgr, SIGNAL (finished (QNetworkReply *)),
                          this , SLOT   (onGotContacts (QNetworkReply *)));
    emit status ("Contacts retrieved, parsing", 0);

    bool rv = false;
    QString msg;
    QDateTime currDT = QDateTime::currentDateTime().toUTC ();
    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();
    dbMain.setLastContactUpdate (currDT);

    QXmlInputSource inputSource;
    QXmlSimpleReader simpleReader;
    ContactsXmlHandler contactsHandler;

    do // Begin cleanup block (not a loop)
    {
        QString strData = reply->readAll ();
        if (strData.contains ("Authorization required"))
        {
            msg = "Authorization failed.";
            break;
        }
        inputSource.setData (strData);

        QObject::connect (&contactsHandler, SIGNAL (log(const QString &, int)),
            this,            SIGNAL (log(const QString &, int)));
        QObject::connect (&contactsHandler, SIGNAL (status(const QString &, int)),
            this,            SIGNAL (status(const QString &, int)));

        QObject::connect (
            &contactsHandler, SIGNAL (oneContact (const ContactInfo &)),
            this,            SLOT   (gotOneContact (const ContactInfo &)));

        simpleReader.setContentHandler (&contactsHandler);
        simpleReader.setErrorHandler (&contactsHandler);

        rv = simpleReader.parse (&inputSource, false);

        msg = QString("Contact parsing done. total = %1. usable = %2")
                .arg (contactsHandler.getTotalContacts ())
                .arg (contactsHandler.getUsableContacts ());
    } while (0); // End cleanup block (not a loop)

    QSqlTableModel *modelContacts = (QSqlTableModel *) ui->treeView->model ();
    if (rv) {
        emit status ("Contacts retrieved. Saving. This will take some time...");
        modelContacts->submitAll ();
        emit status ("Contacts committed to local database");
    } else {
        modelContacts->revertAll ();
    }

    modelContacts->select ();

    emit status (msg);
    emit allContacts (rv);

    reply->deleteLater ();
}//GVContactsTable::onGotContacts

void
GVContactsTable::gotOneContact (const ContactInfo &contactInfo)
{
    CacheDatabase &dbMain = Singletons::getRef().getDBMain ();

    GVContactInfo gvContactInfo;
    convert (contactInfo, gvContactInfo);

    QMutexLocker locker(&mutex);
    QSqlTableModel *modelContacts = (QSqlTableModel *) ui->treeView->model ();

    if (contactInfo.bDeleted)
    {
        dbMain.deleteContact (contactInfo.strId);
        dbMain.deleteContactInfo (contactInfo.strId);
    }
    else    // add or modify
    {
        dbMain.insertContact (modelContacts,
                              contactInfo.strTitle,
                              contactInfo.strId);
        dbMain.putContactInfo (gvContactInfo);
    }

}//GVContactsTable::gotOneContact

bool
GVContactsTable::convert (const ContactInfo &cInfo, GVContactInfo &gvcInfo)
{
    gvcInfo.strLink = cInfo.strId;
    gvcInfo.strName = cInfo.strTitle;

    foreach (PhoneInfo pInfo, cInfo.arrPhones)
    {
        GVContactNumber gvcn;
        switch (pInfo.Type)
        {
        case PType_Mobile:
            gvcn.chType = 'M';
            break;
        case PType_Home:
            gvcn.chType = 'H';
            break;
        case PType_Other:
            gvcn.chType = 'O';
            break;
        default:
            gvcn.chType = '?';
            break;
        }

        gvcn.strNumber = pInfo.strNumber;

        gvcInfo.arrPhones += gvcn;
    }

    return (true);
}//GVContactsTable::convert

void
GVContactsTable::setStatus (const QString &strText, int timeout)
{
    ui->statusbar->showMessage (strText, timeout);
}//GVContactsTable::setStatus
