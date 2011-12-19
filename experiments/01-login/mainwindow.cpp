#include "mainwindow.h"
#include "NwReqTracker.h"

#define GV_LOGIN_1 "https://accounts.google.com/ServiceLogin?nui=5&service=grandcentral&ltmpl=mobile&btmpl=mobile&passive=true&continue=https://www.google.com/voice/m"
#define GV_LOGIN_2 "https://accounts.google.com/ServiceLogin"

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
    QWidget *central = new QWidget(this);
    QGridLayout *layout = new QGridLayout(this);
    plainText = new QPlainTextEdit(this);
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

    QNetworkRequest req(QUrl(GV_LOGIN_1));
    req.setRawHeader("User-Agent", UA_IPHONE4);
    NwReqTracker *tracker = new NwReqTracker(nwMgr.get(req), this);

    bool rv = connect(tracker, SIGNAL(sigDone(bool,const QByteArray &)),
                      this   , SLOT (onLogin1(bool,const QByteArray &)));
    Q_ASSERT(rv);
}//MainWindow::on_actionDo_it

void
MainWindow::onLogin1(bool success,const QByteArray & /*response*/)
{
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        doLogin2 (GV_LOGIN_2);
    } while (0); // End cleanup block (not a loop)
}//MainWindow::onLogin1

bool
MainWindow::doLogin2(QString strUrl)
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

    url.addQueryItem("nui"      , "5");
    url.addQueryItem("service"  , "grandcentral");
    url.addQueryItem("ltmpl"    , "mobile");
    url.addQueryItem("btmpl"    , "mobile");
    url.addQueryItem("passive"  , "true");
    url.addQueryItem("continue" , "https://www.google.com/voice/m");
    url.addQueryItem("timeStmp" , "");
    url.addQueryItem("secTok"   , "");

    url.addQueryItem("GALX"     , galx.value ());

    url.addQueryItem("Email"    , strUser);
    url.addQueryItem("Passwd"   , strPass);

    url.addQueryItem("PersistentCookie" , "yes");
    url.addQueryItem("rmShown"          , "1");
    url.addQueryItem("signIn"           , "Sign+in");

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
}//MainWindow::doLogin2

void
MainWindow::onLogin2(bool success, const QByteArray &response)
{
    bool bLoggedin = false;
    QString strMoved;
    QString strResponse = response;
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        strMoved = isMovedTemporarily (strResponse);
        if (!strMoved.isEmpty ()) {
            doLogin2 (strMoved);
            break;
        }

        Q_DEBUG("And we're back!");

        foreach (QNetworkCookie gvx, jar.getAllCookies ()) {
            if (gvx.name () == "gvx") {
                bLoggedin = true;
                break;
            }
        }

        if (bLoggedin) {
            Q_DEBUG("Login successful!");
        } else {
            Q_DEBUG("Login failed!");
        }
    } while (0); // End cleanup block (not a loop)
}//MainWindow::onLogin2

QString
MainWindow::isMovedTemporarily(const QString &strResponse)
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
}//MainWindow::isMovedTemporarily
