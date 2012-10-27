#include "QGVConnectionManager.h"
#include "QGVConnection.h"
#include "gen/connection_adapter.h"
#include "gen/cm_adapter.h"

#define CM_Param_Flags_Required       1
#define CM_Param_Flags_Register       2
#define CM_Param_Flags_Has_Default    4
#define CM_Param_Flags_Secret         8
#define CM_Param_Flags_DBus_Property 16

#define QGV_ProtocolName "qgv"

QGVConnectionManager::QGVConnectionManager(QObject *parent)
: QObject(parent)
{
}//QGVConnectionManager::QGVConnectionManager

QGVConnectionManager::~QGVConnectionManager()
{
}//QGVConnectionManager::~QGVConnectionManager

bool
QGVConnectionManager::registerObject()
{
    if (NULL == new ConnectionManagerAdaptor(this)) {
        Q_WARN("Failed to allocate CM DBus adapter");
        return false;
    }
    
    QDBusConnection sesionBus = QDBusConnection::sessionBus();
    bool rv = sesionBus.registerService("net.yuvraaj.qgvdial.PhoneIntegration");
    if (rv) {
        Q_WARN("Couldn't register CM service");
        return rv;
    }
    
    rv = sesionBus.registerObject("/", this);
    if (rv) {
        Q_WARN("Couldn't register CM object");
        return rv;
    }
    
    Q_DEBUG("CM object registered");
    
    return rv;
}//QGVConnectionManager::registerObject

void
QGVConnectionManager::unregisterObject()
{
}//QGVConnectionManager::unregisterObject

Qt_Type_a_susv
QGVConnectionManager::GetParameters(const QString &Protocol)
{
    Qt_Type_a_susv rv;
    Struct_susv susv;

    if (Protocol != QGV_ProtocolName) {
        Q_WARN(QString("Invalid protocol: %1").arg(Protocol));
        return rv;
    }

    susv.s1 = "username";   // name
    susv.u  = CM_Param_Flags_Required | CM_Param_Flags_Register;
    susv.s2 = "s";          // signature
    susv.v  = QString();    // default value
    rv.append(susv);

    susv.s1 = "password";   // name
    susv.u  = CM_Param_Flags_Required | CM_Param_Flags_Register |
              CM_Param_Flags_Secret;
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
            Q_WARN(QString("Invalid protocol: %1").arg(Protocol));
            break;
        }

        QString username, password;

        foreach (QString key, Parameters.keys()) {
            if (key == "username") {
                username = Parameters[key].toString();
            } else if (key == "password") {
                password = Parameters[key].toString();
            } else {
                Q_WARN(QString("Unknown parameter key \"%1\"").arg(key));
                // Ignore it. Move on.
            }
        }
        
        // Create the connection objects and associate them together
        QGVConnection *conn = new QGVConnection(this);
        if (NULL == conn) {
            Q_WARN("Failed to create connection object");
            break;
        }
        if (NULL == new ConnectionAdaptor(conn)) {
            Q_WARN("Failed to create connection adapter object");
            delete conn;
            break;
        }

        //TODO: Must emit NewConnection on success
    } while(0); // End cleanup block

    return rv;
}//QGVConnectionManager::RequestConnection
