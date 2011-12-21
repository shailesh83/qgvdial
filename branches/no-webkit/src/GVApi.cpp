#include "GVApi.h"
#include "NwReqTracker.h"

#define GV_ACCOUNT_SERVICELOGIN "https://accounts.google.com/ServiceLogin"

GVApi::GVApi(bool bEmitLog, QObject *parent)
: QObject(parent)
, emitLog(bEmitLog)
, loggedIn(false)
, nwMgr(this)
, jar(this)
{
}//GVApi::GVApi

QString
GVApi::hasMoved(const QString &strResponse)
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
}//GVApi::hasMoved

bool
GVApi::login(AsyncTaskToken *token)
{
    Q_ASSERT(token);
    if (!token) {
        return false;
    }
    if (!token->inParams.contains("user") ||
        !token->inParams.contains("pass")) {
        Q_WARN("Invalid params");
        token->status = ATTS_INVALID_PARAMS;
        return false;
    }

    if (loggedIn) {
        token->status = ATTS_SUCCESS;
        token->emitCompleted ();
        return true;
    }

    QUrl url(GV_ACCOUNT_SERVICELOGIN);
    url.addQueryItem("nui"      , "5");
    url.addQueryItem("service"  , "grandcentral");
    url.addQueryItem("ltmpl"    , "mobile");
    url.addQueryItem("btmpl"    , "mobile");
    url.addQueryItem("passive"  , "true");
    url.addQueryItem("continue" , "https://www.google.com/voice/m");

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", UA_IPHONE4);

    NwReqTracker *tracker = new NwReqTracker(nwMgr.get(req), token,
                                             NW_REPLY_TIMEOUT, emitLog, true,
                                             this);

    bool rv = connect(tracker, SIGNAL(sigDone(bool,const QByteArray&, void*)),
                      this   , SLOT  (onLogin1(bool,const QByteArray&, void*)));
    Q_ASSERT(rv);

    return true;
}//GVApi::login

void
GVApi::onLogin1(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    do { // Begin cleanup block (not a loop)
        if (!success) break;

        postLogin (GV_ACCOUNT_SERVICELOGIN, ctx);
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onLogin1

bool
GVApi::postLogin(QString strUrl, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;

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

    url.addQueryItem("Email"    , token->inParams["user"].toString());
    url.addQueryItem("Passwd"   , token->inParams["pass"].toString());

    url.addQueryItem("PersistentCookie" , "yes");
    url.addQueryItem("rmShown"          , "1");
    url.addQueryItem("signIn"           , "Sign+in");

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", UA_IPHONE4);
    req.setHeader (QNetworkRequest::ContentTypeHeader,
                   "application/octet-stream");
    QNetworkReply *reply = nwMgr.post(req, url.encodedQuery ());
    NwReqTracker *tracker = new NwReqTracker(reply, token, NW_REPLY_TIMEOUT,
                                             emitLog, true, this);

    bool rv = connect(tracker, SIGNAL(sigDone(bool,const QByteArray&, void*)),
                      this   , SLOT  (onLogin2(bool,const QByteArray&, void*)));
    Q_ASSERT(rv);

    return found;
}//GVApi::postLogin

void
GVApi::onLogin2(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strMoved;
        QString strResponse = response;

        strMoved = hasMoved (strResponse);
        if (!strMoved.isEmpty ()) {
            success = postLogin (strMoved, ctx);
            break;
        }

        Q_DEBUG("And we're back!");

        foreach (QNetworkCookie gvx, jar.getAllCookies ()) {
            if (gvx.name () == "gvx") {
                loggedIn = true;
                break;
            }
        }

        success = loggedIn;

        if (success) {
            Q_DEBUG("Login successful!");
        } else {
            Q_DEBUG("Login failed!");
        }

        //TODO: Get the page for _rnr_se
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onLogin2
