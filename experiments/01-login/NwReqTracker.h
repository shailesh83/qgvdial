#ifndef NWREQTRACKER_H
#define NWREQTRACKER_H

#include "global.h"

class NwReqTracker : public QObject
{
    Q_OBJECT
public:
    NwReqTracker(QNetworkReply *r, QObject *parent = 0, bool autoDel = true);
    void abort();

signals:
    void sigDone(bool success, QByteArray response);

private slots:
    void onReplyFinished();
    void onReplyProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onTimedOut();

private:
    QNetworkReply  *reply;
    QTimer          replyTimer;

    bool            aborted;
    bool            autoDelete;
};

#endif // NWREQTRACKER_H
