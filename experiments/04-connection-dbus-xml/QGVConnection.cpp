#include "QGVConnection.h"

QGVConnection::QGVConnection(const QString &u, const QString &p,
                             QObject *parent /*= NULL*/)
: QObject(parent)
, user(u)
, pass(p)
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
