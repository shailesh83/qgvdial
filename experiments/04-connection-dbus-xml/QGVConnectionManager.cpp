#include "QGVConnectionManager.h"

QGVConnectionManager::QGVConnectionManager(QObject *parent)
: QObject(parent)
{
}//QGVConnectionManager::QGVConnectionManager

QGVConnectionManager::~QGVConnectionManager()
{
}//QGVConnectionManager::~QGVConnectionManager

Qt_Type_a_susv
QGVConnectionManager::GetParameters(const QString &Protocol)
{
    Qt_Type_a_susv rv;
    return rv;
}//QGVConnectionManager::GetParameters

QStringList
QGVConnectionManager::ListProtocols()
{
    QStringList rv;
    return rv;
}//QGVConnectionManager::ListProtocols

QString
QGVConnectionManager::RequestConnection(const QString &Protocol,
                                        const Qt_Type_dict_sv &Parameters,
                                        QDBusObjectPath &Object_Path)
{
    QString rv;
    return rv;
}//QGVConnectionManager::RequestConnection
