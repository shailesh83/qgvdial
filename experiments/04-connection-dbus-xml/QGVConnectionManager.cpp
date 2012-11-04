#include "QGVConnectionManager.h"
#include "gen/cm_adapter.h"

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
    foreach (conn, m_connectionMap) {
        delete conn;
    }

    m_connectionMap.clear();
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
        sessionBus.unregisterObject (QGV_CM_OBJECT_PATH);
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
    QGVConnection *conn = NULL;
    bool newConn = false;

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

        if (m_connectionMap.contains (username)) {
            conn = m_connectionMap[username];
            break;
        }

        // Create the connection objects and associate them together
        conn = new QGVConnection(username, password, this);
        if (NULL == conn) {
            Q_WARN("Failed to create connection object");
            break;
        }

        if (!conn->registerObject ()) {
            delete conn;
            conn = NULL;
            break;
        }

        conn->setSelfHandle(++m_connectionHandleCounter);
        m_connectionMap[username] = conn;

        // Must emit NewConnection on success
        newConn = true;
    } while(0); // End cleanup block

    if (NULL != conn) {
        Object_Path = QDBusObjectPath(conn->getDBusObjectPath ());
        rv = conn->getDBusBusName ();

        if (newConn) {
            emit NewConnection(rv, Object_Path, QGV_ProtocolName);
        }
    }

    return rv;
}//QGVConnectionManager::RequestConnection
