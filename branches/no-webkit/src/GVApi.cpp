/*
qgvdial is a cross platform Google Voice Dialer
Copyright (C) 2010  Yuvraaj Kelkar

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Contact: yuvraaj@gmail.com
*/

#include "GVApi.h"
#include "GvXMLParser.h"

GVApi::GVApi(bool bEmitLog, QObject *parent)
: QObject(parent)
, emitLog(bEmitLog)
, loggedIn(false)
, nwMgr(this)
, jar(this)
{
}//GVApi::GVApi

bool
GVApi::getSystemProxies (QNetworkProxy &http, QNetworkProxy &https)
{
#if !DIABLO_OS
    QNetworkProxyFactory::setUseSystemConfiguration (true);
#endif

    do { // Begin cleanup block (not a loop)
        QList<QNetworkProxy> netProxies =
        QNetworkProxyFactory::systemProxyForQuery (
        QNetworkProxyQuery(QUrl("http://www.google.com")));
        http = netProxies[0];
        if (QNetworkProxy::NoProxy != http.type ()) {
            if (emitLog) {
                Q_DEBUG("Got proxy: host = ") << http.hostName ()
                               << ", port = " << http.port ();
            }
            break;
        }

        // Otherwise Confirm it
#if defined(Q_WS_X11)
        QString strHttpProxy = getenv ("http_proxy");
        if (strHttpProxy.isEmpty ()) {
            break;
        }

        int colon = strHttpProxy.lastIndexOf (':');
        if (-1 != colon) {
            QString strHost = strHttpProxy.mid (0, colon);
            QString strPort = strHttpProxy.mid (colon);

            strHost.remove ("http://").remove ("https://");

            strPort.remove (':').remove ('/');
            int port = strPort.toInt ();

            if (emitLog) {
                Q_DEBUG("Found http proxy :") << strHost << ":" << port;
            }
            http.setHostName (strHost);
            http.setPort (port);
            http.setType (QNetworkProxy::HttpProxy);
        }
#endif
    } while (0); // End cleanup block (not a loop)

    do { // Begin cleanup block (not a loop)
        QList<QNetworkProxy> netProxies =
        QNetworkProxyFactory::systemProxyForQuery (
        QNetworkProxyQuery(QUrl("https://www.google.com")));
        https = netProxies[0];
        if (QNetworkProxy::NoProxy != https.type ()) {
            if (emitLog) {
                Q_DEBUG("Got proxy: host =") << https.hostName () << ", port = "
                                             << https.port ();
            }
            break;
        }

        // Otherwise Confirm it
#if defined(Q_WS_X11)
        QString strHttpProxy = getenv ("https_proxy");
        if (strHttpProxy.isEmpty ()) {
            break;
        }

        int colon = strHttpProxy.lastIndexOf (':');
        if (-1 != colon) {
            QString strHost = strHttpProxy.mid (0, colon);
            QString strPort = strHttpProxy.mid (colon);

            strHost.remove ("http://").remove ("https://");

            strPort.remove (':').remove ('/');
            int port = strPort.toInt ();

            if (emitLog) {
                Q_DEBUG("Found http proxy: ") << strHost << ":" << port;
            }
            https.setHostName (strHost);
            https.setPort (port);
            https.setType (QNetworkProxy::HttpProxy);
        }
#endif
    } while (0); // End cleanup block (not a loop)

    return (true);
}//GVApi::getSystemProxies

bool
GVApi::doGet(QUrl url, void *ctx, QObject *receiver, const char *method)
{
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", UA_IPHONE4);

    NwReqTracker *tracker = new NwReqTracker(nwMgr.get(req), ctx,
                                             NW_REPLY_TIMEOUT, emitLog, true,
                                             this);

    bool rv =
    connect(tracker, SIGNAL (sigDone(bool, const QByteArray&, void*)),
            receiver, method);
    Q_ASSERT(rv);

    return rv;
}//GVApi::doGet

bool
GVApi::doGet(const QString &strUrl, void *ctx, QObject *receiver,
             const char *method)
{
    return doGet(QUrl(strUrl), ctx, receiver, method);
}//GVApi::doGet

bool
GVApi::setProxySettings (bool bEnable,
                         bool bUseSystemProxy,
                         const QString &host, int port,
                         bool bRequiresAuth,
                         const QString &user, const QString &pass)
{
    QNetworkProxy proxySettings;
    do // Begin cleanup block (not a loop)
    {
        if (!bEnable) {
            if (emitLog) {
                Q_DEBUG("Clearing all proxy information");
            }
            break;
        }

        if (bUseSystemProxy) {
            QNetworkProxy https;
            getSystemProxies (proxySettings, https);
            if (emitLog) {
                Q_DEBUG("Using system proxy settings");
            }
            break;
        }

        proxySettings.setHostName (host);
        proxySettings.setPort (port);
        proxySettings.setType (QNetworkProxy::HttpProxy);

        if (bRequiresAuth) {
            proxySettings.setUser (user);
            proxySettings.setPassword (pass);
        }

        if (emitLog) {
            Q_DEBUG("Using user defined proxy settings.");
        }
    } while (0); // End cleanup block (not a loop)
    QNetworkProxy::setApplicationProxy (proxySettings);

    return (true);
}//GVApi::setProxySettings

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

        if (emitLog) {
            Q_DEBUG("Moved temporarily to") << rv;
        }
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

    bool rv =
    doGet (url, token, this, SLOT(onLogin1(bool, const QByteArray&, void*)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::login

void
GVApi::onLogin1(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strResponse = response;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        if (!parseHiddenLoginFields (strResponse, hiddenLoginFields)) {
            Q_WARN("Failed to parse hidden fields");
            success = false;
            break;
        }

        success = postLogin (GV_ACCOUNT_SERVICELOGIN, token);
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        Q_WARN("Login failed.") << strResponse;

        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onLogin1

bool
GVApi::parseHiddenLoginFields(const QString &strResponse, QVariantMap &ret)
{
/* To match:
  <input type="hidden" name="continue" id="continue"
           value="https://www.google.com/voice/m" />
*/
    QRegExp rx1("\\<input\\s*type\\s*=\\s*\"hidden\"(.*)\\>");
    rx1.setMinimal (true);
    if (!strResponse.contains (rx1)) {
        Q_WARN("Invalid login page!");
        return false;
    }

    ret.clear ();
    int pos = 0;
    while ((pos = rx1.indexIn (strResponse, pos)) != -1) {
        QString fullMatch = rx1.cap(0);
        QString oneInstance = rx1.cap(1);
        QString name, value;
        QRegExp rx2("\"(.*)\""), rx3("'(.*)'");
        rx2.setMinimal (true);
        rx3.setMinimal (true);

        int pos1 = oneInstance.indexOf ("value");
        if (pos1 == -1) {
            goto gonext;
        }

        name  = oneInstance.left (pos1);
        value = oneInstance.mid (pos1);

        if (rx2.indexIn (name) == -1) {
            goto gonext;
        }
        name = rx2.cap (1);

        if (rx2.indexIn (value) == -1) {
            if (rx3.indexIn (value) == -1) {
                goto gonext;
            } else {
                value = rx3.cap (1);
            }
        } else {
            value = rx2.cap (1);
        }

        ret[name] = value;

gonext:
        pos += fullMatch.indexOf (oneInstance);
    }

    if (ret.count() == 0) {
        Q_WARN("No hidden fields!!");
        return false;
    }

    if (emitLog) {
        Q_DEBUG("login fields =") << ret;
    }

    return true;
}//GVApi::parseHiddenLoginFields

bool
GVApi::postLogin(QString strUrl, void *ctx)
{
    QUrl url(strUrl);
    QNetworkCookie galx;
    bool found = false;
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;

    foreach (QNetworkCookie cookie, jar.getAllCookies ()) {
        if (cookie.name () == "GALX") {
            galx = cookie;
            found = true;
        }
    }

    if (!found) {
        Q_WARN("Invalid cookies. Login failed.");
        return false;
    }

    // HTTPS POST the user credentials along with the cookie values as post data

    QStringList keys;
    QVariantMap allLoginFields;
    allLoginFields["passive"]     = "true";
    allLoginFields["timeStmp"]    = "";
    allLoginFields["secTok"]      = "";
    allLoginFields["GALX"]        = galx.value ();
    allLoginFields["Email"]       = token->inParams["user"];
    allLoginFields["Passwd"]      = token->inParams["pass"];
    allLoginFields["PersistentCookie"] = "yes";
    allLoginFields["rmShown"]     = "1";
    allLoginFields["signIn"]      = "Sign+in";

    keys = hiddenLoginFields.keys();
    foreach (QString key, keys) {
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
                   "application/x-www-form-urlencoded");
    QNetworkReply *reply = nwMgr.post(req, url.encodedQuery ());
    NwReqTracker *tracker = new NwReqTracker(reply, ctx, NW_REPLY_TIMEOUT,
                                             emitLog, this);
    found = connect(tracker, SIGNAL(sigDone(bool,const QByteArray &,void *)),
                    this   , SLOT (onLogin2(bool,const QByteArray &,void *)));
    Q_ASSERT(found);

    return found;
}//GVApi::postLogin

void
GVApi::onLogin2(bool success, const QByteArray &response, void *ctx)
{
    QString strResponse = response;
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        // There will be 2-3 moved temporarily redirects.
        QString strMoved = hasMoved(strResponse);
        if (!strMoved.isEmpty ()) {
            if (strMoved.contains ("smsAuth", Qt::CaseInsensitive)) {
                if (emitLog) {
                    Q_DEBUG("Two factor AUTH required!");
                }
                success = beginTwoFactorAuth (strMoved, token);
            } else {
                success = postLogin (strMoved, token);
            }
            break;
        }

        // After this we should have completed login. Check for coolie "gvx"
        foreach (QNetworkCookie cookie, jar.getAllCookies ()) {
            if (cookie.name () == "gvx") {
                loggedIn = true;
                break;
            }
        }

        // If "gvx" was found, then we're logged in.
        if (!loggedIn) {
            Q_WARN("Login failed!") << strResponse;
            success = false;
            break;
        }

        success = getRnr (token);
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        Q_WARN("Login failed.") << strResponse;

        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onLogin2

bool
GVApi::beginTwoFactorAuth(const QString &strUrl, void *ctx)
{
    QUrl url(strUrl);
    url.addQueryItem("service", "grandcentral");

    bool rv = doGet(url, ctx, this,
                    SLOT(onTwoFactorLogin(bool, const QByteArray &, void *)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::beginTwoFactorAuth

void
GVApi::onTwoFactorLogin(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strResponse = response;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strMoved = hasMoved (strResponse);
        if (!strMoved.isEmpty ()) {
            success = beginTwoFactorAuth (strMoved, token);
            break;
        }

        Q_DEBUG(strResponse);

        // After which we should have completed login. Check for coolie "gvx"
        success = false;
        foreach (QNetworkCookie gvx, jar.getAllCookies ()) {
            if (gvx.name () == "gvx") {
                success = true;
                break;
            }
        }

        if (success) {
            Q_DEBUG("Login succeeded");
            token->status = ATTS_SUCCESS;
            token->emitCompleted ();
            break;
        }

        success = doTwoFactorAuth (strResponse, token);
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        Q_WARN("Login failed.") << strResponse;

        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onTwoFactorLogin

bool
GVApi::doTwoFactorAuth(const QString &strResponse, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QNetworkCookie galx;
    bool foundgalx = false;
    bool rv = false;

    do { // Begin cleanup block (not a loop)
        foreach (QNetworkCookie cookie, jar.getAllCookies ()) {
            if (cookie.name () == "GALX") {
                galx = cookie;
                foundgalx = true;
            }
        }

        QVariantMap ret;
        if (!parseHiddenLoginFields (strResponse, ret)) {
            break;
        }

        if (!ret.contains ("smsToken")) {
            // It isn't two factor authentication
            Q_WARN("Username or password is incorrect!");
            break;
        }

        if (!foundgalx) {
            Q_WARN("Cannot proceed with two factor auth. Giving up");
            break;
        }

        QString smsUserPin;
        emit twoStepAuthentication(token, smsUserPin);
        if (smsUserPin.isEmpty ()) {
            Q_WARN("User didn't enter user pin");
            break;
        }

        QUrl url(GV_ACCOUNT_SMSAUTH), url1(GV_ACCOUNT_SMSAUTH);
        url.addQueryItem("service"          , "grandcentral");

        url1.addQueryItem("smsUserPin"      , smsUserPin);
        url1.addQueryItem("smsVerifyPin"    , "Verify");
        url1.addQueryItem("PersistentCookie", "yes");
        url1.addQueryItem("service"         , "grandcentral");
        url1.addQueryItem("GALX"            , galx.value());

        QStringList keys = ret.keys ();
        foreach (QString key, keys) {
            url1.addQueryItem(key, ret[key].toString());
        }

        QNetworkRequest req(url);
        req.setRawHeader("User-Agent", UA_IPHONE4);
        req.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");
        QNetworkReply *reply = nwMgr.post(req, url1.encodedQuery ());
        NwReqTracker *tracker = new NwReqTracker(reply, this);

        rv =
        connect(tracker, SIGNAL    (sigDone(bool, const QByteArray &, void *)),
                this   , SLOT(onTFAAutoPost(bool, const QByteArray &, void *)));
        Q_ASSERT(rv);
    } while (0); // End cleanup block (not a loop)

    return (rv);
}//GVApi::doTwoFactorAuth

void
GVApi::onTFAAutoPost(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strResponse = response;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        success = false;
        QRegExp rx("<form\\s*action\\s*=\\s*\"(.*)\"\\s*method\\s*=\\s*\"POST\"");
        if ((rx.indexIn (strResponse) == -1) || (rx.numCaptures () != 1)) {
            Q_WARN("Failed to login.");
            break;
        }

        QString nextUrl = rx.cap(1);

        QVariantMap ret;
        if (!parseHiddenLoginFields (strResponse, ret)) {
            Q_WARN("Failed to login.");
            break;
        }

        QStringList keys;
        keys = ret.keys();
        foreach (QString key, keys) {
            hiddenLoginFields[key] = ret[key];
        }

        success = postLogin (nextUrl, token);
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        Q_WARN("Login failed.") << strResponse;

        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onTFAAutoPost

bool
GVApi::getRnr(void *ctx)
{
    bool rv = doGet("https://www.google.com/voice/m/i/all", ctx, this,
                    SLOT (onGotRnr(bool, const QByteArray &, void *)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::getRnr

void
GVApi::onGotRnr(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strResponse = response;

    do { // Begin cleanup block (not a loop)
        if (!success) break;

        QString strMoved = hasMoved (strResponse);
        if (!strMoved.isEmpty ()) {
            success = doGet(strMoved, ctx, this,
                            SLOT(onGotRnr(bool,const QByteArray &)));
            Q_ASSERT(success);
            break;
        }

        success = false;
        int pos = strResponse.indexOf ("_rnr_se");
        if (pos == -1) {
            break;
        }

        int pos1 = strResponse.indexOf (">", pos);
        if (pos1 == -1) {
            break;
        }

        QString searchIn = strResponse.mid (pos, pos1-pos);
        QRegExp rx("value\\s*=\\s*\\\"(.*)\\\"");

        if (rx.indexIn (searchIn) == -1) {
            break;
        }

        rnr_se = rx.cap (1);

        token->status = ATTS_SUCCESS;
        token->emitCompleted ();

        success = true;
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        Q_WARN("Login failed.") << strResponse;

        token->status = ATTS_LOGIN_FAILURE;
        token->emitCompleted ();
    }
}//GVApi::onGotRnr

bool
GVApi::logout(AsyncTaskToken *token)
{
    bool rv = doGet(GV_HTTPS "/account/signout", token, this,
                    SLOT (onLogout(bool, const QByteArray&, void*)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::logout

void
GVApi::onLogout(bool /*success*/, const QByteArray & /*response*/, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    token->status = ATTS_SUCCESS;
    token->emitCompleted ();;
}//GVApi::onLogout

bool
GVApi::getPhones(AsyncTaskToken *token)
{
    bool rv = doGet (GV_HTTPS "/b/0/settings/tab/phones", token, this,
                     SLOT (onLogout(bool, const QByteArray&, void*)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::getPhones

void
GVApi::onGetPhones(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strReply = response;

    do { // Begin cleanup block (not a loop)
        if (!success) {
            Q_WARN("Failed to get phones");
            break;
        }
        success = false;

        QString strMoved = hasMoved (strReply);
        if (!strMoved.isEmpty ()) {
            success = doGet (strMoved, token, this,
                             SLOT(onGetPhones(bool, const QByteArray&, void*)));
            break;
        }

        QXmlInputSource inputSource;
        QXmlSimpleReader simpleReader;
        inputSource.setData (strReply);
        GvXMLParser xmlHandler;
        xmlHandler.setEmitLog (emitLog);

        simpleReader.setContentHandler (&xmlHandler);
        simpleReader.setErrorHandler (&xmlHandler);

        simpleReader.parse (&inputSource, false);

        QString strTemp;
        QScriptEngine scriptEngine;
        strTemp = "var topObj = " + xmlHandler.strJson;
        scriptEngine.evaluate (strTemp);
        if (scriptEngine.hasUncaughtException ()) {
            strTemp = QString ("Could not assign json to topObj : %1")
                      .arg (scriptEngine.uncaughtException().toString());
            qWarning() << strTemp;
            if (emitLog) {
                Q_DEBUG("Data from GV:") << strReply;
            }
            break;
        }

        strSelfNumber =
        scriptEngine.evaluate("topObj[\"settings\"][\"primaryDid\"]").toString();
        if (scriptEngine.hasUncaughtException ()) {
            strTemp = QString ("Could not parse primaryDid from topObj : %1")
                      .arg (scriptEngine.uncaughtException().toString());
            qWarning() << strTemp;
            if (emitLog) {
                Q_DEBUG("Data from GV:") << strReply;
            }
            break;
        }

        token->outParams["self_number"] = strSelfNumber;

        if ("CLIENT_ONLY" == strSelfNumber) {
            qWarning ("This account has not been configured. No phone calls possible.");
        }

        strTemp = "var phoneParams = []; "
                  "var phoneList = []; "
                  "for (var phoneId in topObj[\"phones\"]) { "
                  "    phoneList.push(phoneId); "
                  "}";
        scriptEngine.evaluate (strTemp);
        if (scriptEngine.hasUncaughtException ()) {
            strTemp = QString ("Uncaught exception executing script : %1")
                      .arg (scriptEngine.uncaughtException().toString());
            qWarning() << strTemp;
            if (emitLog) {
                Q_DEBUG("Data from GV:") << strReply;
            }
            break;
        }

        qint32 nPhoneCount = scriptEngine.evaluate("phoneList.length;").toInt32 ();
        if (emitLog) {
            Q_DEBUG("phone count =") << nPhoneCount;
        }

        for (qint32 i = 0; i < nPhoneCount; i++) {
            strTemp = QString(
                    "phoneParams = []; "
                    "for (var params in topObj[\"phones\"][phoneList[%1]]) { "
                    "    phoneParams.push(params); "
                    "}").arg(i);
            scriptEngine.evaluate (strTemp);
            if (scriptEngine.hasUncaughtException ()) {
                strTemp = QString ("Uncaught exception in phone loop: %1")
                          .arg (scriptEngine.uncaughtException().toString());
                qWarning() << strTemp;
                if (emitLog) {
                    Q_DEBUG("Data from GV:") << strReply;
                }
                break;
            }

            qint32 nParams =
            scriptEngine.evaluate ("phoneParams.length;").toInt32 ();

            GVRegisteredNumber regNumber;
            for (qint32 j = 0; j < nParams; j++) {
                strTemp = QString("phoneParams[%1];").arg (j);
                QString strPName = scriptEngine.evaluate (strTemp).toString ();
                strTemp = QString(
                          "topObj[\"phones\"][phoneList[%1]][phoneParams[%2]];")
                            .arg (i)
                            .arg (j);
                QString strVal = scriptEngine.evaluate (strTemp).toString ();
                if (scriptEngine.hasUncaughtException ()) {
                    strTemp =
                    QString ("Uncaught exception in phone params loop: %1")
                            .arg (scriptEngine.uncaughtException().toString());
                    qWarning() << strTemp;
                    if (emitLog) {
                        Q_DEBUG("Data from GV:") << strReply;
                    }
                    break;
                }

                if (strPName == "id") {
                    regNumber.strId = strVal;
                } else if (strPName == "name") {
                    regNumber.strName = strVal;
                } else if (strPName == "phoneNumber") {
                    regNumber.strNumber = strVal;
                } else if (strPName == "type") {
                    regNumber.chType = strVal[0].toAscii ();
                } else if ((strPName == "verified") ||
                           (strPName == "policyBitmask") ||
                           (strPName == "dEPRECATEDDisabled") ||
                           (strPName == "telephonyVerified") ||
                           (strPName == "smsEnabled") ||
                           (strPName == "incomingAccessNumber") ||
                           (strPName == "voicemailForwardingVerified") ||
                           (strPName == "behaviorOnRedirect") ||
                           (strPName == "carrier") ||
                           (strPName == "customOverrideState") ||
                           (strPName == "inVerification") ||
                           (strPName == "recentlyProvisionedOrDeprovisioned") ||
                           (strPName == "formattedNumber") ||
                           (strPName == "wd") ||
                           (strPName == "we") ||
                           (strPName == "scheduleSet") ||
                           (strPName == "weekdayAllDay") ||
                           (strPName == "weekdayTimes") ||
                           (strPName == "weekendAllDay") ||
                           (strPName == "weekendTimes") ||
                           (strPName == "redirectToVoicemail") ||
                           (strPName == "active") ||
                           (strPName == "enabledForOthers")) {
                } else {
                    if (emitLog) {
                        Q_DEBUG(QString ("param = %1. value = %2")
                                    .arg (strPName).arg (strVal));
                    }
                }
            }

            if (emitLog) {
                Q_DEBUG("Name =") << regNumber.strName
                    << "number =" << regNumber.strNumber
                    << "type ="   << regNumber.chType;
            }
            emit registeredPhone (regNumber);
        }

        token->status = ATTS_SUCCESS;
        token->emitCompleted ();;

        success = true;
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        token->status = ATTS_FAILURE;
        token->emitCompleted ();;
    }
}//GVApi::onGetPhones

bool
GVApi::getInbox(AsyncTaskToken *token)
{
    if (!token) return false;

    // Ensure that the params  are valid
    if (!token->inParams.contains ("type") ||
        !token->inParams.contains ("page"))
    {
        token->status = ATTS_INVALID_PARAMS;
        token->emitCompleted ();
        return true;
    }

    QString strLink = QString (GV_HTTPS "/b/0/inbox/recent/%1?page=p%2")
                        .arg(token->inParams["type"].toString())
                        .arg(token->inParams["page"].toString());

    bool rv = doGet (strLink, token, this,
                     SLOT(onGetInbox(bool, const QByteArray&, void*)));
    Q_ASSERT(rv);

    return rv;
}//GVApi::getInbox

void
GVApi::onGetInbox(bool success, const QByteArray &response, void *ctx)
{
    AsyncTaskToken *token = (AsyncTaskToken *)ctx;
    QString strReply = response;

    do { // Begin cleanup block (not a loop)
        if (!success) {
            Q_WARN("Failed to get inbox");
            break;
        }
        success = false;

        QString strMoved = hasMoved (strReply);
        if (!strMoved.isEmpty ()) {
            success = doGet (strMoved, token, this,
                             SLOT(onGetInbox(bool, const QByteArray&, void*)));
            break;
        }

        QXmlInputSource inputSource;
        QXmlSimpleReader simpleReader;
        inputSource.setData (strReply);
        GvXMLParser xmlHandler;
        xmlHandler.setEmitLog (emitLog);

        simpleReader.setContentHandler (&xmlHandler);
        simpleReader.setErrorHandler (&xmlHandler);

        if (emitLog) {
            Q_DEBUG("Begin parsing");
        }
        if (!simpleReader.parse (&inputSource, false)) {
            Q_WARN("Failed to parse GV Inbox XML. Data =") << strReply;
            break;
        }
        if (emitLog) {
            Q_DEBUG("End parsing");
        }

        /*
        QDateTime dtUpdate = workCurrent.arrParams[3].toDateTime ();
        bool bGotOld = false;
        int nNew = 0;
        qint32 nUsableMsgs = 0;
        if (!parseInboxJson (dtUpdate, xmlHandler.strJson, xmlHandler.strHtml,
                             bGotOld, nNew, nUsableMsgs))
        {
            Q_WARN("Failed to parse GV Inbox JSON. Data =") << strReply;
            break;
        }
        if (workCurrent.arrParams.count() < 5) {
            workCurrent.arrParams.append (nNew);
        } else {
            nNew += workCurrent.arrParams[4].toInt();
            workCurrent.arrParams[4] = nNew;
        }

        QMutexLocker locker(&mutex);
        nCurrent++;

        bOk = true;

        int count = workCurrent.arrParams[2].toString().toInt ();
        if (((nCurrent-nFirstPage) >= count) ||
            (bGotOld) ||
            (0 == nUsableMsgs)) {
            completeCurrentWork (GVAW_getInbox, true);
            break;
        }
        */
    } while (0); // End cleanup block (not a loop)

    if (!success) {
        token->status = ATTS_FAILURE;
        token->emitCompleted ();;
    }
}//GVApi::onGetInbox
