#include "MainObject.h"

#define CONTENT_IS_FORM "application/x-www-form-urlencoded"
#define CONTENT_IS_TEXT "text/plain"

MainObject::MainObject(QObject *parent)
: QObject(parent)
{
    connect(&mgr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onFinished(QNetworkReply*)));

    QTimer::singleShot(10, this, SLOT(doWork()));
}//MainObject::MainObject

void
MainObject::doWork()
{
    QUrl url("http://ec2-50-18-18-251.us-west-1.compute.amazonaws.com/pytest/getMonth.py");
    url.addQueryItem("name", "uv");

    QNetworkRequest req(url);
    req.setHeader (QNetworkRequest::ContentTypeHeader, CONTENT_IS_FORM);
    reply = mgr.post(req, url.encodedQuery());
}//MainObject::doWork

void
MainObject::doWork1()
{
    QUrl url("http://ec2-50-18-18-251.us-west-1.compute.amazonaws.com/");

    QNetworkRequest req(url);
    reply = mgr.get(req);
}//MainObject::doWork

void
MainObject::onFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->error();
    }

    QString tmp = reply->readAll();
    qDebug() << tmp;

    qApp->quit();
}//MainObject::onFinished

