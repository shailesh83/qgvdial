#ifndef MAINOBJECT_H
#define MAINOBJECT_H

#include <QtCore>
#include <QtNetwork>

class MainObject : public QObject {
    Q_OBJECT
public:
    MainObject(QObject *parent = NULL);

private slots:
    void doWork();
    void doWork1();

    void onFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager mgr;
    QNetworkReply *reply;
};

#endif//MAINOBJECT_H

