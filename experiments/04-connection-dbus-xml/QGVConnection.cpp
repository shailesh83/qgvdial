#include "QGVConnection.h"
#include "gen/connection_adapter.h"

QGVConnection::QGVConnection(const QString &u, const QString &p,
                             QObject *parent /*= NULL*/)
: QObject(parent)
, m_user(u)
, m_pass(p)
{
}//QGVConnection::QGVConnection

QGVConnection::~QGVConnection()
{
}//QGVConnection::~QGVConnection

void
QGVConnection::AddClientInterest(const QStringList &Tokens)
{
}//QGVConnection::AddClientInterest

void
QGVConnection::Connect()
{
}//QGVConnection::Connect

void
QGVConnection::Disconnect()
{
}//QGVConnection::Disconnect

QStringList
QGVConnection::GetInterfaces()
{
    QStringList rv;
    return rv;
}//QGVConnection::GetInterfaces

QString
QGVConnection::GetProtocol()
{
    return QGV_ProtocolName;
}//QGVConnection::GetProtocol

uint
QGVConnection::GetSelfHandle()
{
    return m_selfHandle;
}//QGVConnection::GetSelfHandle

uint
QGVConnection::GetStatus()
{
    uint rv;
    return rv;
}//QGVConnection::GetStatus

void
QGVConnection::HoldHandles(uint Handle_Type, const Qt_Type_au &Handles)
{
}//QGVConnection::HoldHandles

QStringList
QGVConnection::InspectHandles(uint Handle_Type, const Qt_Type_au &Handles)
{
    QStringList rv;
    return rv;
}//QGVConnection::InspectHandles

Qt_Type_a_osuu
QGVConnection::ListChannels()
{
    Qt_Type_a_osuu rv;
    return rv;
}//QGVConnection::ListChannels

void
QGVConnection::ReleaseHandles(uint Handle_Type, const Qt_Type_au &Handles)
{
}//QGVConnection::ReleaseHandles

void
QGVConnection::RemoveClientInterest(const QStringList &Tokens)
{
}//QGVConnection::RemoveClientInterest

QDBusObjectPath
QGVConnection::RequestChannel(const QString &Type, uint Handle_Type, uint Handle,
                              bool Suppress_Handler)
{
    QDBusObjectPath rv;
    return rv;
}//QGVConnection::RequestChannel

Qt_Type_au
QGVConnection::RequestHandles(uint Handle_Type, const QStringList &Identifiers)
{
    Qt_Type_au rv;
    return rv;
}//QGVConnection::RequestHandles

void
QGVConnection::setSelfHandle(uint h)
{
    m_selfHandle = h;
}//QGVConnection::SetSelfHandle

int
QGVConnection::getSelfHandle()
{
    return m_selfHandle;
}//QGVConnection::getSelfHandle

QString
QGVConnection::getDBusObjectPath()
{
    return m_dbusObjectPath;
}//QGVConnection::getDBusObjectPath

QString
QGVConnection::getDBusBusName()
{
    return m_dbusBusName;
}//QGVConnection::getDBusBusName

bool
QGVConnection::registerObject()
{
    if (NULL == new ConnectionAdaptor(this)) {
        Q_WARN("Failed to create connection adapter object");
        return false;
    }

    m_dbusObjectPath = QGV_CONN_OBJECT_PREFIX + m_user;
    m_dbusObjectPath.replace('@', '_');

    m_dbusBusName = QGV_CONN_SERVICE_PREFIX + m_user;
    m_dbusBusName.replace('@', '_');

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    bool rv = sessionBus.registerObject(m_dbusObjectPath, this);

    do { // Begin cleanup block (not a loop)
        if (!rv) {
            Q_WARN("Couldn't register Connection object to user ") << m_user;
            break;
        }
        rv = sessionBus.registerService (m_dbusBusName);
        if (!rv) {
            Q_WARN("Couldn't register Connection bus for user ") << m_user;
            sessionBus.unregisterObject (m_dbusObjectPath);
            break;
        }

        Q_DEBUG("Connection registered for user ") << m_user;
    } while (0); // End cleanup block (not a loop)

    if (!rv) {
        m_dbusObjectPath.clear ();
        m_dbusBusName.clear ();
    }

    return rv;
}//QGVConnection::registerObject

bool
QGVConnection::unregisterObject()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
}//QGVConnection::unregisterObject
