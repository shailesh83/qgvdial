#ifndef GVAPI_H
#define GVAPI_H

#include "global.h"
#include "CookieJar.h"
#include <QObject>

class GVApi : public QObject
{
    Q_OBJECT
public:
    GVApi(bool bEmitLog, QObject *parent = 0);

    bool login(AsyncTaskToken *token);

signals:

private slots:
    void onLogin1(bool success, const QByteArray &response, void *ctx);
    void onLogin2(bool success, const QByteArray &response, void *ctx);

private:
    QString hasMoved(const QString &strResponse);
    bool postLogin(QString strUrl, void *ctx);

private:
    bool emitLog;
    bool loggedIn;

    QNetworkAccessManager nwMgr;
    CookieJar jar;
};

#endif // GVAPI_H