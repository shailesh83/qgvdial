#include "mainwindow.h"
#include "NwReqTracker.h"

#define GV_LOGIN_1 "https://accounts.google.com/ServiceLogin?nui=5&service=grandcentral&ltmpl=mobile&btmpl=mobile&passive=true&continue=https://www.google.com/voice/m"
#define GV_ACCOUNT_SERVICELOGIN "https://accounts.google.com/ServiceLogin"
#define GV_ACCOUNT_SMSAUTH      "https://accounts.google.com/SmsAuth"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, plainText(NULL)
, strUser("yuvraaj@gmail.com")
, nwMgr(this)
, jar(this)
, logsMutex(QMutex::Recursive)
, logsTimer(this)
{
    bool rv;

    plainText = new QPlainTextEdit(this);

    QWidget *central = new QWidget(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget (plainText, 0,0);

    central->setLayout (layout);
    this->setCentralWidget (central);

    QMenuBar *menuBar = this->menuBar ();
    QMenu *mnuFile = menuBar->addMenu ("&File");
    QAction *actDoIt = mnuFile->addAction ("Do it");
    QAction *actExit = mnuFile->addAction ("E&xit");

    actDoIt->setShortcut (QKeySequence("Ctrl+D"));
    actExit->setShortcut (QKeySequence("Ctrl+Q"));

    rv = connect(actDoIt, SIGNAL(triggered()), this, SLOT(on_actionDo_it()));
    Q_ASSERT(rv);
    rv = connect(actExit, SIGNAL(triggered()), this, SLOT(on_actionExit()));
    Q_ASSERT(rv);

    nwMgr.setCookieJar(&jar);

    rv = connect(&logsTimer, SIGNAL(timeout()), this, SLOT(onLogsTimer()));
    Q_ASSERT(rv);

    logsTimer.setSingleShot (false);
    logsTimer.setInterval (3000);;
    logsTimer.start ();

    QTimer::singleShot (1000, this, SLOT(on_actionDo_it()));
}//MainWindow::MainWindow

MainWindow::~MainWindow()
{
    if (plainText) {
        delete plainText;
        plainText = NULL;
    }
}//MainWindow::~MainWindow

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
//    showFullScreen();
    show();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void
MainWindow::log(const QString &strLog)
{
    QMutexLocker locker(&logsMutex);
    logsList.append (strLog);
}//MainWindow::log

void
MainWindow::onLogsTimer()
{
    if (!plainText) {
        return;
    }

    QMutexLocker locker(&logsMutex);
    foreach (QString strLog, logsList) {
        plainText->appendPlainText(strLog);
    }
    logsList.clear ();

    logsTimer.start ();
}//MainWindow::onLogsTimer

void
MainWindow::on_actionExit()
{
    qApp->quit ();
}//MainWindow::on_actionExit

void
MainWindow::on_actionDo_it()
{
    strUser = QInputDialog::getText(this, "Get User", "Enter user",
                                    QLineEdit::Normal, strUser);
    strPass = QInputDialog::getText(this, "Password", "Enter pass",
                                    QLineEdit::Password, strPass);

    // First, HTTP GET the Service login page. This loads up the cookies.

    QUrl url(GV_ACCOUNT_SERVICELOGIN);
    url.addQueryItem("nui"      , "5");
    url.addQueryItem("service"  , "grandcentral");
    url.addQueryItem("ltmpl"    , "mobile");
    url.addQueryItem("btmpl"    , "mobile");
    url.addQueryItem("passive"  , "true");
    url.addQueryItem("continue" , "https://www.google.com/voice/m");

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", UA_IPHONE4);
    NwReqTracker *tracker = new NwReqTracker(nwMgr.get(req), this);

    bool rv = connect(tracker, SIGNAL(sigDone(bool,const QByteArray &)),
                      this   , SLOT (onLogin1(bool,const QByteArray &)));
    Q_ASSERT(rv);
}//MainWindow::on_actionDo_it

QString
MainWindow::hasMoved(const QString &strResponse)
{
    QString rv;
    do { // Begin cleanup block (not a loop)
        if (!strResponse.contains ("Moved Temporarily")) {
            break;
        }

        QRegExp rx("a\\s+href=\"(.*)\"\\>", Qt::CaseInsensitive);
        rx.setMinimal (true);
        if (!strResponse.contains (rx) || (rx.captureCount () != 1)) {
            break;
        }

        rv = rx.cap(1);
        Q_DEBUG("Moved temporarily to") << rv;
    } while (0); // End cleanup block (not a loop)

    return rv;
}//MainWindow::hasMoved

void
MainWindow::onLogin1(bool success,const QByteArray & response)
{
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strResponse = response;
        Q_DEBUG(strResponse);

        if (!parseHiddenLoginFields (strResponse)) {
            break;
        }

        postLogin (GV_ACCOUNT_SERVICELOGIN);
    } while (0); // End cleanup block (not a loop)
}//MainWindow::onLogin1

bool
MainWindow::parseHiddenLoginFields(const QString &strResponse)
{
/* To match:
  <input type="hidden" name="continue" id="continue"
           value="https://www.google.com/voice/m" />
*/
    QRegExp rx1("<input\\s*type\\s*=\\s*\"hidden\"\\s*(.*)\\s*/>");
    rx1.setMinimal (true);
    if (!strResponse.contains (rx1)) {
        Q_WARN("Invalid login page!");
        return false;
    }

    hiddenLoginFields.clear ();
    int pos = 0;
    while ((pos = rx1.indexIn (strResponse, pos)) != -1) {
        QRegExp rx2("name\\s*=\\s*\\\"(.*)\\\".*id=.*value\\s*=\\s*\\\"(.*)\\\"");
        rx2.setMinimal (true);

        QString oneInstance = rx1.cap (0);
        if (!oneInstance.contains(rx2) || (rx2.captureCount() != 2)) {
            pos += oneInstance.length();
            continue;
        }

        Q_DEBUG(rx2.cap (1)) << "=" << rx2.cap (2);

        hiddenLoginFields[rx2.cap (1)] = rx2.cap (2);

        pos += oneInstance.length();
    }

    if (hiddenLoginFields.count() == 0) {
        Q_WARN("Invalid login page!");
        return false;
    }

    Q_DEBUG("login fields =") << hiddenLoginFields;

    return true;
}//MainWindow::parseHiddenLoginFields

bool
MainWindow::postLogin(QString strUrl)
{
    QUrl url(strUrl);
    QNetworkCookie galx;
    bool found = false;

    foreach (QNetworkCookie cookie, jar.getAllCookies ()) {
        if (cookie.name () == "GALX") {
            galx = cookie;
            found = true;
        }
    }

    if (!found) return false;

    // HTTPS POST the user credentials along with the cookie values as post data

    QStringList keys;
    QVariantMap allLoginFields;
    allLoginFields["passive"]     = "true";
    allLoginFields["timeStmp"]    = "";
    allLoginFields["secTok"]      = "";
    allLoginFields["GALX"]        = galx.value ();
    allLoginFields["Email"]       = strUser;
    allLoginFields["Passwd"]      = strPass;
    allLoginFields["PersistentCookie"] = "yes";
    allLoginFields["rmShown"]     = "1";
    allLoginFields["signIn"]      = "Sign+in";

    keys = hiddenLoginFields.keys();
    foreach (QString key, keys) {
        Q_DEBUG("key =") << key << ", value =" << hiddenLoginFields[key];
        allLoginFields[key] = hiddenLoginFields[key];
    }

    keys = allLoginFields.keys();
    foreach (QString key, keys) {
        if (key != "dsh") {
            url.addQueryItem(key, allLoginFields[key].toString());
        }
    }

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", UA_IPHONE4);
    req.setHeader (QNetworkRequest::ContentTypeHeader,
                   "application/octet-stream");
    QNetworkReply *reply = nwMgr.post(req, url.encodedQuery ());
    NwReqTracker *tracker = new NwReqTracker(reply, this);

    found = connect(tracker, SIGNAL(sigDone(bool,const QByteArray &)),
                    this   , SLOT (onLogin2(bool,const QByteArray &)));
    Q_ASSERT(found);

    return found;
}//MainWindow::postLogin

void
MainWindow::onLogin2(bool success, const QByteArray &response)
{
    bool bLoggedin = false;
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strMoved;
        QString strResponse = response;
        Q_DEBUG(strResponse);

        // There will be 2-3 moved temporarily redirects.
        strMoved = hasMoved(strResponse);
        if (!strMoved.isEmpty ()) {
            postLogin (strMoved);
            break;
        }

        Q_DEBUG("And we're back!");

        // After which we should have completed login. Check for coolie "gvx"
        foreach (QNetworkCookie gvx, jar.getAllCookies ()) {
            if (gvx.name () == "gvx") {
                bLoggedin = true;
                break;
            }
        }

        // If "gvx" was found, then we're logged in.
        if (bLoggedin) {
            Q_DEBUG("Login successful!");
            break;
        }

        Q_DEBUG("Not logged in yet. Response =") << strResponse;

        // Normal login has failed. We check for this regex to see if the user
        // has set up two factor authentication.
        QRegExp rx("\\<input.*name\\s*=\\s*\\\"smsToken\\\"\\s*"
                   "value\\s*=\\s*\\\"(.*)\\\"");
        if (!strResponse.contains (rx) || (rx.captureCount () != 1)) {
            // It isn't two factor authentication
            Q_WARN("Username or password is incorrect!");
            break;
        }

        // Two factor auth! Pull out the SMS token.
        QString smsToken = rx.cap (1);

        QString smsUserPin =
        QInputDialog::getText(this, "Two factor authentication",
                              "Enter token", QLineEdit::Normal);
        if (smsUserPin.isEmpty ()) {
            Q_WARN("User didn't enter user pin");
            break;
        }

        QUrl url(GV_ACCOUNT_SMSAUTH), url1(GV_ACCOUNT_SMSAUTH);
        url.addQueryItem("service"  , "grandcentral");

        url1.addQueryItem("timeStmp"    , "");
        url1.addQueryItem("secTok"      , "");
        url1.addQueryItem("smsToken"    , smsToken);
        url1.addQueryItem("email"       , strUser);
        url1.addQueryItem("smsUserPin"  , smsUserPin);
        url1.addQueryItem("smsVerifyPin", "Verify");
        url1.addQueryItem("PersistentCookie", "yes");

        Q_DEBUG(QVariant(url).toString ());
        Q_DEBUG(QVariant(url1).toString ());

        QNetworkRequest req(url);
        req.setRawHeader("User-Agent", UA_IPHONE4);
        req.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/octet-stream");
        QNetworkReply *reply = nwMgr.post(req, url1.encodedQuery ());
        NwReqTracker *tracker = new NwReqTracker(reply, this);

        success =
        connect(tracker, SIGNAL(sigDone(bool,const QByteArray &)),
                this   , SLOT  (onTwoFactorLogin(bool,const QByteArray &)));
        Q_ASSERT(success);

    } while (0); // End cleanup block (not a loop)
}//MainWindow::onLogin2

void
MainWindow::onTwoFactorLogin(bool success, const QByteArray &response)
{
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strResponse = response;
        Q_DEBUG(strResponse);

//        postLogin (GV_ACCOUNT_SERVICELOGIN);
    } while (0); // End cleanup block (not a loop)
}//MainWindow::onTwoFactorLogin
