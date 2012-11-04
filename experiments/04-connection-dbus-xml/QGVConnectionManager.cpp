#include "QGVConnectionManager.h"
#include "gen/connection_adapter.h"
#include "gen/cm_adapter.h"

#define QGV_CM_OBJECT_PATH  TP_CM_OBJECT_PATH  "/qgvtp"
#define QGV_CM_SERVICE_PATH TP_CM_SERVICE_PATH ".qgvtp"

#define CM_Param_Flags_Required       1
#define CM_Param_Flags_Register       2
#define CM_Param_Flags_Has_Default    4
#define CM_Param_Flags_Secret         8
#define CM_Param_Flags_DBus_Property 16

QGVConnectionManager::QGVConnectionManager(QObject *parent)
: QObject(parent)
, m_connectionHandleCounter(0)
{
}//QGVConnectionManager::QGVConnectionManager

QGVConnectionManager::~QGVConnectionManager()
{
    QGVConnection *conn;
    foreach (conn, m_connectionList) {
        delete conn;
    }

    m_connectionList.clear();
}//QGVConnectionManager::~QGVConnectionManager

bool
QGVConnectionManager::registerObject()
{
    if (NULL == new ConnectionManagerAdaptor(this)) {
        Q_WARN("Failed to allocate CM DBus adapter");
        return false;
    }

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    bool rv = sessionBus.registerObject(QGV_CM_OBJECT_PATH, this);
    if (!rv) {
        Q_WARN("Couldn't register CM object");
        return rv;
    }
    rv = sessionBus.registerService (QGV_CM_SERVICE_PATH);
    if (!rv) {
        Q_WARN("Couldn't register CM object");
        return rv;
    }

    Q_DEBUG("CM object registered");

    return rv;
}//QGVConnectionManager::registerObject

void
QGVConnectionManager::unregisterObject()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    sessionBus.unregisterService (QGV_CM_SERVICE_PATH);
    sessionBus.unregisterObject(QGV_CM_OBJECT_PATH);
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

    // I could have used "username", but "account" is the well known name for
    // this parameter
    susv.s1 = "account";
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
            if (key == "account") {
                username = Parameters[key].toString();
            } else if (key == "password") {
                password = Parameters[key].toString();
            } else {
                Q_WARN(QString("Unknown parameter key \"%1\"").arg(key));
                // Ignore it. Move on.
            }
        }

        // Create the connection objects and associate them together
        QGVConnection *conn = new QGVConnection(username, password, this);
        if (NULL == conn) {
            Q_WARN("Failed to create connection object");
            break;
        }
        if (NULL == new ConnectionAdaptor(conn)) {
            Q_WARN("Failed to create connection adapter object");
            delete conn;
            break;
        }

        conn->setSelfHandle(++m_connectionHandleCounter);
        m_connectionList.append(conn);

        //TODO: Set up object path from the connection object that we just got.
        // Object_Path =

        //TODO: Must emit NewConnection on success
        emit NewConnection(QString(), Object_Path, QString());
    } while(0); // End cleanup block

    return rv;
}//QGVConnectionManager::RequestConnection
