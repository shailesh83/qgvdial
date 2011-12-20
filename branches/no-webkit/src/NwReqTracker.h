#ifndef NWREQTRACKER_H
#define NWREQTRACKER_H

#include "global.h"
#include <QObject>

#define NW_REPLY_TIMEOUT (30 * 1000)

class NwReqTracker : public QObject
{
    Q_OBJECT
public:
    NwReqTracker(QNetworkReply *r, quint32 timeout = NW_REPLY_TIMEOUT,
                 bool bEmitlog = true, bool autoDel = true,
                 QObject *parent = 0);
    void abort();
    void setTimeout(quint32 timeout);

signals:
    void sigDone(bool success, QByteArray response);

private slots:
    void onReplyFinished();
    void onReplyProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onTimedOut();

    void onReplySslErrors (const QList<QSslError> &errors);
    void onReplyError(QNetworkReply::NetworkError code);

    void onXferProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkReply  *reply;
    QTimer          replyTimer;

    bool            aborted;
    bool            autoDelete;
    bool            emitLog;
};

#endif // NWREQTRACKER_H
