#include "QGVConnectionManager.h"

#define Conn_Mgr_Param_Flags_Required       1
#define Conn_Mgr_Param_Flags_Register       2
#define Conn_Mgr_Param_Flags_Has_Default    4
#define Conn_Mgr_Param_Flags_Secret         8
#define Conn_Mgr_Param_Flags_DBus_Property 16

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
    Struct_susv susv;
    
    susv.s1 = "user";
    susv.u  = Conn_Mgr_Param_Flags_Required | Conn_Mgr_Param_Flags_Register;
    susv.s2 = "String containing the DBus signature";
    susv.v  = QString();
    rv.append(susv);

    susv.s1 = "pass";
    susv.u  = Conn_Mgr_Param_Flags_Required | Conn_Mgr_Param_Flags_Register |
              Conn_Mgr_Param_Flags_Secret;
    susv.s2 = "String containing the DBus signature";
    susv.v  = QString();
    rv.append(susv);
    
    return rv;
}//QGVConnectionManager::GetParameters

QStringList
QGVConnectionManager::ListProtocols()
{
    QStringList rv;
    rv.append("qgv");
    return rv;
}//QGVConnectionManager::ListProtocols

QString
QGVConnectionManager::RequestConnection(const QString &Protocol,
                                        const Qt_Type_dict_sv &Parameters,
                                        QDBusObjectPath &Object_Path)
{
    QString rv;
    //TODO: Must emit NewConnection on success
    return rv;
}//QGVConnectionManager::RequestConnection
