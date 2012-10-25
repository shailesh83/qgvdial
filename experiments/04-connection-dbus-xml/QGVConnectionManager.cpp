#include "QGVConnectionManager.h"

#define Conn_Mgr_Param_Flags_Required       1
#define Conn_Mgr_Param_Flags_Register       2
#define Conn_Mgr_Param_Flags_Has_Default    4
#define Conn_Mgr_Param_Flags_Secret         8
#define Conn_Mgr_Param_Flags_DBus_Property 16

#define QGV_ProtocolName "qgv"

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
    
    if (Protocol != QGV_ProtocolName) {
        return rv;
    }
    
    susv.s1 = "username";   // name
    susv.u  = Conn_Mgr_Param_Flags_Required | Conn_Mgr_Param_Flags_Register;
    susv.s2 = "s";          // signature
    susv.v  = QString();    // default value
    rv.append(susv);

    susv.s1 = "password";   // name
    susv.u  = Conn_Mgr_Param_Flags_Required | Conn_Mgr_Param_Flags_Register |
              Conn_Mgr_Param_Flags_Secret;
    susv.s2 = "s";          // signature
    susv.v  = QString();    // default value
    rv.append(susv);
    
    return rv;
}//QGVConnectionManager::GetParameters

QStringList
QGVConnectionManager::ListProtocols()
{
    QStringList rv;
    rv.append(QGV_ProtocolName);
    return rv;
}//QGVConnectionManager::ListProtocols

QString
QGVConnectionManager::RequestConnection(const QString &Protocol,
                                        const QVariantMap &Parameters,
                                        QDBusObjectPath &Object_Path)
{
    QString rv;

    do { // Begin cleanup block
        if (Protocol != QGV_ProtocolName) {
            break;
        }
    
        QString username, password;
    
        foreach (QString key, Parameters.keys()) {
            if (key == "username") {
                username = Parameters[key].toString();
            } else if (key == "password") {
                password = Parameters[key].toString();
            } else {
                qWarning() << "Unknown parameter key \"" << key << "\"";
                // Ignore it. Move on.
            }
        }

        //TODO: Must emit NewConnection on success
    } while(0); // End cleanup block
    
    return rv;
}//QGVConnectionManager::RequestConnection
