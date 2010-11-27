/*
 * Telepathy SNOM VoIP phone connection manager
 * Copyright (C) 2006 by basyskom GmbH
 *  @author Tobias Hunger <info@basyskom.de>
 *
 * This library is free software; you can redisQObject::tribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is disQObject::tributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin SQObject::treet, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "connection.h"

#include "connectionadaptor.h"
#include "connectioninterfaceprivacyadaptor.h"
#include "channel.h"
#include "phoneconnector.h"

#include <QtCore>
#include <QtNetwork>

namespace
{
static const QString protocol_snom("tel");

static const QString connection_service_name_prefix("org.freedesktop.Telepathy.Connection." TP_NAME "." + protocol_snom + '.');
static const QString connection_object_path_prefix("/org/freedesktop/Telepathy/Connection/" TP_NAME "/" + protocol_snom + '/');
static const QString requests_interface("org.freedesktop.Telepathy.Connection.Interface.Requests");

static const QString privacy_dnd_disabled("allow-all");
static const QString privacy_dnd_enabled("allow-subscribed");
static const QString privacy_interface("org.freedesktop.Telepathy.Connection.Interface.Privacy");
}


class ConnectionPrivate
{
public:
    ConnectionPrivate(Connection * p,
                      PhoneConnector * c,
                      const QString & acc) :
        connector(c),
        account(acc),
        connection_status(Connection::Disconnected),
        adaptor(new ConnectionAdaptor(p)),
        privacyAdaptor(new PrivacyAdaptor(p)),
        nextContactHandle(1),
        doNotDisturb(false),
        parent(p)
    {
        Q_ASSERT(0 != adaptor);
        Q_ASSERT(0 != connector);
    }

    ~ConnectionPrivate()
    {
        qDebug() << "CPriv: Known contacts:" << knownContacts;
        qDebug() << "CPriv: usage count:" << contactUsageCount;
    }

    Channel * findChannel(const uint handle_id)
    {
        Channel * channel;
        foreach (channel, channels)
        {
            if (channel->hasOpposite(handle_id))
            {
                qDebug() << "CPriv: Channel found!";
                return channel;
            }
        }
        return 0;
    }

    Channel * createChannel()
    {
        Channel * new_channel = new Channel(parent);
        Q_ASSERT(0 != new_channel);

        if (!new_channel->registerObject())
        {
            // FIXME: Which error do I need to return here?
            delete new_channel;
            return 0;
        }

        QObject::connect(new_channel, SIGNAL(Closed()), parent, SLOT(doChannelClosed()));

        channels.append(new_channel);

        return new_channel;
    }

    PhoneConnector * const connector;

    const QString account;

    Connection::Status connection_status;

    ConnectionAdaptor * adaptor;
    PrivacyAdaptor * privacyAdaptor;

    uint nextContactHandle;
    QHash<uint, QString> knownContacts;
    QHash<uint, uint> contactUsageCount;

    QList<Channel *> channels;

    bool doNotDisturb;

    Connection * const parent;
};

// ---------------------------------------------------------------------------

Connection::Connection(PhoneConnector * const pc,
                        const QString & account,
                        QObject * parent) :
    QObject(parent),
    d(new ConnectionPrivate(this, pc, account))
{
    Q_ASSERT(0 != d);
    Q_ASSERT(!account.isEmpty());

    d->connector->addConnection(this);
    qDebug() << "Connection set up.";
}

Connection::~Connection()
{
    d->connector->rmConnection(this);
    delete(d);
}

bool Connection::registerObject()
{
    if (!QDBusConnection::sessionBus().registerService(serviceName()))
    {
        qDebug() << "Problem registering connection service:" << serviceName();
        return false;
    }

    if (!QDBusConnection::sessionBus().registerObject(objectPath().path(),
                                                      this))
    {
        qDebug() << "Problem registering object path:" << objectPath().path();
        return false;
    }
    return true;
}

void Connection::unregisterObject()
{
    QDBusConnection::sessionBus().unregisterObject(objectPath().path());
    QDBusConnection::sessionBus().unregisterService(serviceName());
}

QString Connection::name() const
{
    // Make a cleanish connection name:
    QString connection_name(protocol_snom + '_');
    connection_name += d->account + "@" + d->connector->name();

    connection_name = connection_name.replace('@', "_40");
    connection_name = connection_name.replace('.', "_2e");
    connection_name = connection_name.replace(':', "_3a");

#if not defined (Q_NO_DEBUG)
    {
        QRegExp regexp("[^a-zA-Z0-9_-]");
        Q_ASSERT(-1 == connection_name.indexOf(regexp));
    }
#endif

    return connection_name;
}


QString Connection::serviceName() const
{ return connection_service_name_prefix + name(); }

QDBusObjectPath Connection::objectPath() const
{ return QDBusObjectPath(connection_object_path_prefix + name()); }

bool Connection::handlesAccount(const QString & account) const
{ return account.startsWith(d->account); }

void Connection::Connect()
{
    if (!d->connector->isConnected()) { return; }
    qDebug() << "C: Connect...";
    d->connection_status = Connection::Connected;
    uint handle(findHandle(d->account));
    Q_ASSERT(0 != handle);
    qDebug() << "C: Emitting StatusChanged.";
    emit StatusChanged(d->connection_status, ReasonRequested);
    qDebug() << "C: Connect done.";
}

void Connection::Disconnect()
{
    qDebug() << "C: Disconnect...";
    QList<uint> handles;
    handles.append(findHandleInternal(d->account));
    Q_ASSERT(handles[0] != 0);
    ReleaseHandles(HandleContact, handles);

    d->connection_status = Disconnected;
    qDebug() << "C: Emitting StatusChanged";
    emit StatusChanged(d->connection_status, ReasonRequested);

    unregisterObject();

    deleteLater();
}

QStringList Connection::GetInterfaces()
{
    QStringList result;
    if (d->connection_status != Connected)
    {
        // FIXME: return org.freedesktop.Telepathy.Error.Disconnected
        return result;
    }
    result << privacy_interface;
    return result;
}

QString Connection::GetProtocol()
{ return protocol_snom; }

uint Connection::GetStatus()
{ return static_cast<uint>(d->connection_status); }

uint Connection::GetSelfHandle()
{
    qDebug() << "C: GetSelfHandle";
    if (d->connection_status != Connected)
    {
        // FIXME: return org.freedesktop.Telepathy.Error.Disconnected
        qDebug() << "NOT CONNECTED when requesting selfhandle!";
        return 0;
    }
    uint handle = findHandleInternal(d->account);
    Q_ASSERT(0 != handle);
    qDebug() << "Returning Handle" << handle << "as self handle.";
    return handle;
}

QList<uint> Connection::RequestHandles(uint handle_type,
                                       const QStringList & names)
{
    QList<uint> result;

    // check input:
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return result;
    }
    if (handle_type != HandleContact)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.InvalidArgument.
        return result;
    }

    // process input:
    QString name;
    foreach (name, names)
    {
        uint this_contact_handle(d->nextContactHandle);
        ++(d->nextContactHandle);
        Q_ASSERT(!d->knownContacts.contains(this_contact_handle));

        d->knownContacts.insert(this_contact_handle, name);
        d->contactUsageCount.insert(this_contact_handle, 1);
        result.append(this_contact_handle);

        Q_ASSERT(d->contactUsageCount.contains(this_contact_handle) &&
                 d->knownContacts.contains(this_contact_handle));
        Q_ASSERT(1 == d->contactUsageCount.value(this_contact_handle));

        qDebug() << "C: request handle:" << this_contact_handle
                 << "value:" << d->knownContacts.value(this_contact_handle)
                 << "count:" << d->contactUsageCount.value(this_contact_handle);

    }

    return result;
}

void Connection::HoldHandles(const uint handle_type, const QList<uint> &handles)
{
    qDebug() << "C: HoldHandles.";
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return;
    }
    if (handle_type != HandleContact)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.InvalidArgument.
        return;
    }

    uint handle;
    foreach(handle, handles)
    {
        if (!d->knownContacts.contains(handle)) { continue; }
        Q_ASSERT(d->contactUsageCount.contains(handle));

        d->contactUsageCount.insert(handle,
                                    d->contactUsageCount.value(handle) + 1);

        Q_ASSERT(d->contactUsageCount.value(handle) > 1);
        qDebug() << "hold handle:" << handle
                 << "value:" << d->knownContacts.value(handle)
                 << "count:" << d->contactUsageCount.value(handle);
    }

}

QStringList Connection::InspectHandles(const uint handle_type,
                                       const QList<uint> &handles)
{
    QStringList result;

        // check input:
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return result;
    }
    if (handle_type != HandleContact)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.InvalidArgument.
        return result;
    }

    // process input:
    uint handle;
    foreach (handle, handles)
    {
        if (d->knownContacts.contains(handle))
        { result.append(d->knownContacts.value(handle)); }
        else
        { result.append(QString()); }
    }

    return result;
}

void Connection::ReleaseHandles(const uint handle_type, const QList<uint> &handles)
{
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        qDebug() << "Releasing Handle while connection is no longer connected.";
        return;
    }
    if (handle_type != HandleContact)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.InvalidArgument.
        qDebug() << "Trying to release a Handle that is not a contact.";
        return;
    }

    uint handle;
    foreach(handle, handles)
    {
        if (!d->knownContacts.contains(handle)) { continue; }

        Q_ASSERT(d->contactUsageCount.contains(handle));

        uint usageCount = d->contactUsageCount.value(handle);
        Q_ASSERT(usageCount > 0);
        --usageCount;

        if (usageCount == 0)
        {
            d->knownContacts.remove(handle);
            d->contactUsageCount.remove(handle);
            qDebug() << "release handle:" << handle << "Removed!";
        }
        else
        {
            d->contactUsageCount.insert(handle, usageCount);
            qDebug() << "release handle:" << handle
                 << "value:" << d->knownContacts.value(handle)
                 << "count:" << d->contactUsageCount.value(handle);
        }
    }
}

org::freedesktop::Telepathy::ChannelInfoList Connection::ListChannels()
{
    org::freedesktop::Telepathy::ChannelInfoList result;
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return result;
    }

    Channel * channel;
    foreach (channel, d->channels)
    {
        org::freedesktop::Telepathy::ChannelInfo info;
        info.objectPath = channel->objectPath();
        info.interfaceName = channel->GetChannelType();
        uint handle_id;
        info.handleType = channel->GetHandle(handle_id);
        info.handle = handle_id;
        result.append(info);
    }
    return result;
}

QDBusObjectPath Connection::RequestChannel(const QString &type,
                                           uint handle_type, uint handle,
                                           bool suppress_handler)
{
    if (type != QString("org.freedesktop.Telepathy.Channel.Type.StreamedMedia"))
    {
        sendErrorReply("org.freedesktop.Telepathy.Error.NotImplemented",
                       "Failed to create channel: Channel type not implemented.");
        return QDBusObjectPath();
    }
    if ((handle_type == HandleContact && !d->knownContacts.contains(handle)) ||
        (handle_type != 0 && handle == 0))
    {
        sendErrorReply("org.freedesktop.Telepathy.Error.InvalidHandle",
                       "Failed to create channel: Handle type not supported.");
        return QDBusObjectPath();
    }
    if (d->connection_status != Connected)
    {
        sendErrorReply("org.freedesktop.Telepathy.Error.Disconnected",
                       "Failed to create channel: Connection is Disconnected.");
        return QDBusObjectPath();
    }

    qDebug() << "C: Creating Channel object.";
    Channel * new_channel(0);
    new_channel = new Channel(this);

    if (0 == new_channel)
    {
        qDebug() << "C: Failed to create Channel with" << handle_type << handle;
        return QDBusObjectPath();
    }

    qDebug() << "C: Channel created.";
    if (!new_channel->registerObject())
    {
        // FIXME: Which error do I need to return here?
        qDebug() << "C: Failed to register Channel object with DBus.";
        delete new_channel;
        return QDBusObjectPath();
    }
    qDebug() << "C: Channel registered with D-Bus.";
    d->channels.append(new_channel);

    connect(new_channel, SIGNAL(Closed()), this, SLOT(doChannelClosed()));

    qDebug() << "C: Channel info is registered now.";

    new_channel->outgoingChannel(handle);

    qDebug() << "C: Emitting NewChannel.";
    emit NewChannel(new_channel->objectPath(), type,
                    handle_type, handle,
                    suppress_handler);
    qDebug() << "C: Channel signals were connected.";

    if (handle_type == 0)
    { return new_channel->objectPath(); }
    else if (!new_channel->connectOutgoingChannel())
    {
        // FIXME: Which error do I need to return here?
        delete new_channel;
        return QDBusObjectPath();
    }

    qDebug() << "C: Channel is connected.";

    return new_channel->objectPath();
}

void Connection::doChannelClosed()
{
    Channel * channel = static_cast<Channel *>(sender());
    if (d->channels.indexOf(channel) >= 0)
    {
        d->channels.removeAll(channel);
        channel->unregisterObject();
        qDebug() << "C: Channel unregistered...";
    }
}

void Connection::incomingCall(const uint local_handle, const uint remote_handle)
{
    qDebug() << "C: incoming call:" << local_handle << remote_handle;

    Q_ASSERT(d->knownContacts[local_handle].startsWith(d->account));
    Channel * new_channel = d->findChannel(remote_handle);
    Q_ASSERT(0 == new_channel);

    new_channel = d->createChannel();

    new_channel->incomingChannel(remote_handle);

    qDebug() << "C: Emitting NewChannel.";
    emit NewChannel(new_channel->objectPath(),
                    QString("org.freedesktop.Telepathy.Channel.Type.StreamedMedia"),
                    HandleContact, remote_handle,
                    false);
}

void Connection::outgoingCall(const uint local_handle, const uint remote_handle)
{
    qDebug() << "C: outgoing call:" << local_handle << remote_handle;

    Q_ASSERT(d->knownContacts[local_handle].startsWith(d->account));
    Channel * channel = d->findChannel(remote_handle);
    if (0 == channel)
    {
        channel = d->createChannel();
        channel->outgoingChannel(remote_handle);
        qDebug() << "C: Emitting NewChannel.";
        emit NewChannel(channel->objectPath(),
                        QString("org.freedesktop.Telepathy.Channel.Type.StreamedMedia"),
                        HandleContact, remote_handle,
                        false);
    }
    Q_ASSERT(0 != channel);
}

PhoneConnector * Connection::connector() const
{ return d->connector; }

uint Connection::findHandle(const QString & uri)
{
    qDebug() << "C: findHandle" << uri;
    uint handle = findHandleInternal(uri);
    if (handle != 0)
    {
        QList<uint> handle_list;
        handle_list.append(handle);
        HoldHandles(Connection::HandleContact, handle_list);
        return handle;
    }
    else
    {
        QStringList uris(uri);
        QList<uint> result(RequestHandles(Connection::HandleContact, uris));
        if (result.size() != 1) { return 0; }
        return result[0];
    }
}

uint Connection::findHandleInternal(const QString & uri) const
{
    uint handle;
    foreach(handle, d->knownContacts.keys())
    {
        bool found = ( d->knownContacts.value(handle).startsWith(uri) ||
                       uri.startsWith(d->knownContacts.value(handle)) );
        qDebug() << "C: Comparing" << d->knownContacts.value(handle)
                    << "and" << uri << ":" << found;
        if (found)
        { return handle; }
    }
    return 0;
}

void Connection::setConnected(const uint local_handle, const uint remote_handle,
                              const bool value)
{
    qDebug() << "C: set Connected:" << local_handle << remote_handle << value;

    Q_ASSERT(d->knownContacts[local_handle].startsWith(d->account));
    Channel * channel = d->findChannel(remote_handle);
    Q_ASSERT(0 != channel);

    org::freedesktop::Telepathy::StreamState state =
            org::freedesktop::Telepathy::STREAM_STATE_DISCONNECTED;
    if (value) { state = org::freedesktop::Telepathy::STREAM_STATE_CONNECTED;}

    channel->setStreamState(state);

    if (!value)
    {
        d->channels.removeAll(channel);
        channel->unregisterObject();
    }
}

// Connection.Interface.Privacy:

// helper:
void Connection::setDND(const bool dnd_state)
{
    if (dnd_state == d->doNotDisturb) { return; }

    d->doNotDisturb = dnd_state;

    qDebug() << "C: Emitting PrivacyModeChanged.";
    emit PrivacyModeChanged(getPrivacyModeString());
}

QString Connection::getPrivacyModeString() const
{
    if (d->doNotDisturb) { return privacy_dnd_enabled; }
    else { return privacy_dnd_disabled; }
}

// Interface:
QString Connection::GetPrivacyMode()
{
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return QString();
    }

    return getPrivacyModeString();
}

QStringList Connection::GetPrivacyModes()
{
    QStringList result;
    result << privacy_dnd_disabled << privacy_dnd_enabled;
    return result;
}

void Connection::SetPrivacyMode(const QString &mode)
{
    if (d->connection_status != Connected)
    {
        // FIXME: raise org.freedesktop.Telepathy.Error.Disconnected
        return;
    }

    if ((mode == privacy_dnd_enabled && !d->doNotDisturb) ||
        (mode == privacy_dnd_disabled && d->doNotDisturb))
    { connector()->sendKey(key_dnd); }
}
