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

#include "channel.h"

#include "channeladaptor.h"
#include "channelstreamedmediaadaptor.h"
#include "channelinterfacegroupadaptor.h"
#include "channelgroupinterfacetypes.h"
#include "channelinterfacedtmfadaptor.h"
#include "channelinterfacedtmftypes.h"
#include "connection.h"
#include "phoneconnector.h"

#include <QtCore/QDebug>
#include <QtNetwork/QHttp>

class ChannelPrivate
{
public:
    ChannelPrivate(Connection * c,
                   Channel * parent) :
        adaptor(new ChannelAdaptor(parent)),
        smAdaptor(new StreamedMediaAdaptor(parent)),
        groupAdaptor(new GroupAdaptor(parent)),
        dtmfAdaptor(new DTMFAdaptor(parent)),
        connection(c),
        state(org::freedesktop::Telepathy::STREAM_STATE_CONNECTING)
    {
        Q_ASSERT(0 != adaptor);
        Q_ASSERT(0 != smAdaptor);
        Q_ASSERT(0 != groupAdaptor);
        Q_ASSERT(0 != dtmfAdaptor);
        Q_ASSERT(0 != connection);
    }

    ~ChannelPrivate()
    { }

    ChannelAdaptor * const adaptor;
    StreamedMediaAdaptor * const smAdaptor;
    GroupAdaptor * const groupAdaptor;
    DTMFAdaptor * const dtmfAdaptor;

    QHttp protocol;

    Connection * const connection;

    org::freedesktop::Telepathy::StreamState state;

    QList<uint> local_pending_members;
    QList<uint> remote_pending_members;
    QList<uint> confirmed_members;
};

// ---------------------------------------------------------------------------

Channel::Channel(Connection * c, QObject * parent)
: QObject(parent)
, d(new ChannelPrivate(c, this))
{
    Q_ASSERT(0 != d);
    qDebug() << "Ch: created...";
}

Channel::~Channel()
{
    QList<uint> handles;
    handles += d->local_pending_members;
    handles += d->remote_pending_members;
    handles += d->confirmed_members;

    d->connection->ReleaseHandles(Connection::HandleContact, handles);
    QDBusConnection::sessionBus().unregisterObject(objectPath().path());
    qDebug() << "Ch: Destructing!";
    delete(d);
}

void Channel::outgoingChannel(const uint handle)
{
    qDebug() << "Ch: outgoing Channel to" << handle;
    if (handle != 0)
    { d->remote_pending_members.append(handle); }
    qDebug() << "Ch: remote_pending handles:" << d->remote_pending_members;
    d->confirmed_members.append(GetSelfHandle());
    qDebug() << "Ch: confirmed_handles:" << d->confirmed_members;
    Q_ASSERT(d->confirmed_members.size() == 1);
    Q_ASSERT(d->local_pending_members.isEmpty());
}

bool Channel::connectOutgoingChannel()
{
    qDebug() << "Ch: Connecting outgoing channel!";
    QList<uint> handles;
    handles.append(d->remote_pending_members[0]);
    handles.append(d->confirmed_members[0]);
    QStringList config = d->connection->InspectHandles(uint(Connection::HandleContact),
                                                       handles);
    Q_ASSERT(config.size() == 2);
    bool result = d->connection->connector()->dial(config[0], config[1]);

    d->state = org::freedesktop::Telepathy::STREAM_STATE_CONNECTING;
    qDebug() << "Ch: emitting StreamStateChanged.";
    emit StreamStateChanged(1, d->state);
    qDebug() << "Ch: dialed..." << result;

    return result;
}

void Channel::incomingChannel(const uint handle)
{
    qDebug() << "Ch: incoming Channel from" << handle;
    d->local_pending_members.append(GetSelfHandle());
    qDebug() << "Ch: local_pending handles:" << d->local_pending_members;
    d->confirmed_members.append(handle);
    qDebug() << "Ch: confirmed handles:" << d->confirmed_members;
    Q_ASSERT(d->local_pending_members.size() == 1);
    Q_ASSERT(d->confirmed_members.size() == 1);
    Q_ASSERT(d->remote_pending_members.isEmpty());
}

bool Channel::registerObject()
{
    if (!QDBusConnection::sessionBus().registerObject(objectPath().path(),
                                                      this))
    {
        qDebug() << "Problem registering object path:" << objectPath().path();
        return false;
    }
    qDebug() << "Ch: emitting StreamAdded";
    emit StreamAdded(1, 0, org::freedesktop::Telepathy::STREAM_TYPE_AUDIO);
    return true;
}

void Channel::unregisterObject()
{
    emit Closed();
    deleteLater();
}

QString Channel::serviceName() const
{ return d->connection->serviceName(); }

QDBusObjectPath Channel::objectPath() const
{ return QDBusObjectPath(d->connection->objectPath().path() + '/' + name()); }

QString Channel::name() const
{ return QString("smChannel" + QString::number(ulong(d))); }

void Channel::setStreamState(const org::freedesktop::Telepathy::StreamState state)
{
    if (d->state == state)
    { return; }

    if (state == org::freedesktop::Telepathy::STREAM_STATE_CONNECTED)
    {
        qDebug() << "Ch: setting stream state to connected!";
        QList<uint> added_members;

        added_members += d->remote_pending_members;
        added_members += d->local_pending_members;
        d->confirmed_members += added_members;

        d->remote_pending_members.clear();
        d->local_pending_members.clear();

        qDebug() << "Ch: member lists updated: remote_pending:" << d->remote_pending_members
                 << "local_pending:" << d->local_pending_members
                 << "members:" << d->confirmed_members;
        if (!added_members.isEmpty())
        {
            qDebug() << "Ch: Emitting MembersChanged.";
            emit MembersChanged(QString(""), added_members, QList<uint>(),
                                QList<uint>(), QList<uint>(),
                                GetSelfHandle(), 0);
        }
    }
    if (state == org::freedesktop::Telepathy::STREAM_STATE_DISCONNECTED)
    {
        qDebug() << "Ch: setting stream state to disconnected!";
        QList<uint> removed_members(d->confirmed_members);
        d->confirmed_members.clear();
        d->local_pending_members.clear();
        d->remote_pending_members.clear();

        qDebug() << "Ch: member lists updated: remote_pending:" << d->remote_pending_members
                 << "local_pendidng:" << d->local_pending_members
                 << "members:" << d->confirmed_members;
        qDebug() << "Ch: Emitting MembersChanged.";
        emit MembersChanged(QString(""), QList<uint>(), removed_members,
                            QList<uint>(), QList<uint>(),
                            GetSelfHandle(), 0);

        d->connection->connector()->sendKey(key_hangup);
    }

    qDebug() << "Ch: Changing streamstate from" << d->state << "to" << state;
    d->state = state;
    qDebug() << "Ch: emmitting StreamStateChanged";
    emit StreamStateChanged(1, d->state);
}

void Channel::Close()
{
    qDebug() << "Ch: Close() called.";

    d->connection->connector()->sendKey(key_cancel);
}

QString Channel::GetChannelType() const
{ return QString("org.freedesktop.Telepathy.Channel.Type.StreamedMedia"); }

uint Channel::GetHandle(uint &id) const
{
    id = 0;
    return 0;
}

QStringList Channel::GetInterfaces() const
{
    QStringList result;
    result << "org.freedesktop.Telepathy.Channel.Interface.Group";
    result << "org.freedesktop.Telepathy.Channel.Interface.DTMF";
    return result;
}

// Channel.StreamedMedia:

org::freedesktop::Telepathy::StreamInfoList Channel::ListStreams()
{
    org::freedesktop::Telepathy::StreamInfoList result;
    org::freedesktop::Telepathy::StreamInfo info;
    info.id = 1;
    info.contactHandle = 0;
    info.type = org::freedesktop::Telepathy::STREAM_TYPE_AUDIO;
    info.direction = org::freedesktop::Telepathy::STREAM_DIRECTION_BIDIRECTIONAL;
    info.pendingFlags = d->state;
    result.append(info);
    return result;
}

void Channel::RemoveStreams(org::freedesktop::Telepathy::StreamTypeList streams)
{ }

void Channel::RequestStreamDirection(uint stream_id, uint stream_direction)
{ }

org::freedesktop::Telepathy::StreamInfoList
Channel::RequestStreams(uint contact_handle, org::freedesktop::Telepathy::StreamTypeList types)
{
    org::freedesktop::Telepathy::StreamInfoList result;
    return result;
}

// Channel.Interface.DTMF
void Channel::StartTone(uint stream_id, uchar event)
{
    if (stream_id != 1)
    {
        sendErrorReply("org.freedesktop.Telepathy.Error.InvalidArgument",
                       tr("Stream id is invalid."));
        return;
    }

    switch(event)
    {
        case org::freedesktop::Telepathy::Digit_0:
            d->connection->connector()->sendKey(key_digit_0);
            return;
        case org::freedesktop::Telepathy::Digit_1:
            d->connection->connector()->sendKey(key_digit_1);
            return;
        case org::freedesktop::Telepathy::Digit_2:
            d->connection->connector()->sendKey(key_digit_2);
            return;
        case org::freedesktop::Telepathy::Digit_3:
            d->connection->connector()->sendKey(key_digit_3);
            return;
        case org::freedesktop::Telepathy::Digit_4:
            d->connection->connector()->sendKey(key_digit_4);
            return;
        case org::freedesktop::Telepathy::Digit_5:
            d->connection->connector()->sendKey(key_digit_5);
            return;
        case org::freedesktop::Telepathy::Digit_6:
            d->connection->connector()->sendKey(key_digit_6);
            return;
        case org::freedesktop::Telepathy::Digit_7:
            d->connection->connector()->sendKey(key_digit_7);
            return;
        case org::freedesktop::Telepathy::Digit_8:
            d->connection->connector()->sendKey(key_digit_8);
            return;
        case org::freedesktop::Telepathy::Digit_9:
            d->connection->connector()->sendKey(key_digit_9);
            return;
        case org::freedesktop::Telepathy::Asterisk:
            d->connection->connector()->sendKey(key_asterisk);
            return;
        case org::freedesktop::Telepathy::Hash:
            d->connection->connector()->sendKey(key_hash);
            return;
        default:
            sendErrorReply("org.freedesktop.Telepathy.Error.NotAvailable",
                           tr("Event not available on this phone."));
            return;
    }
}

void Channel::StopTone(uint stream_id)
{
    sendErrorReply("org.freedesktop.Telepathy.Error.NotAvailable",
                   tr("This phone does support fixed length DTMF tones only."));
}

// Channel.Interface.Group

// Used to accept incoming calls ONLY!
void Channel::AddMembers(const QList<uint> &contacts,
                         const QString & /* message */)
{
    qDebug() << "Ch: adding members:" << contacts;
    QList<uint> added_members;
    uint contact;
    foreach (contact, contacts)
    {
        if (d->local_pending_members.contains(contact))
        {
            qDebug() << "Ch: moving" << contact << "from local_pending to members.";
            d->local_pending_members.removeAll(contact);
            d->confirmed_members.append(contact);
            added_members.append(contact);
        }

        if (!added_members.isEmpty())
        {
            qDebug() << "Ch: Emitting MembersChanged.";
            emit MembersChanged(QString(""), added_members, QList<uint>(),
                                QList<uint>(), QList<uint>(),
                                GetSelfHandle(), 0);
        }

        if (d->confirmed_members.size() >= 2 &&
            d->state != org::freedesktop::Telepathy::STREAM_STATE_CONNECTED)
        {
            d->state = org::freedesktop::Telepathy::STREAM_STATE_CONNECTED;
            qDebug() << "Ch: emitting StreamStateChanged.";
            emit StreamStateChanged(1, d->state);
            qDebug() << "Ch: dialed...";

            d->connection->connector()->sendKey(key_pickup);
        }
    }
}

QList<uint> Channel::GetAllMembers(QList<uint> &local_pending,
                                   QList<uint> &remote_pending)
{
    local_pending = d->local_pending_members;
    remote_pending = d->remote_pending_members;
    return d->confirmed_members;
}

uint Channel::GetGroupFlags()
{
    return (org::freedesktop::Telepathy::CHANNEL_GROUP_FLAG_CAN_REMOVE |
            org::freedesktop::Telepathy::CHANNEL_GROUP_FLAG_HANDLE_OWNERS_NOT_AVAILABLE);
}

QList<uint> Channel::GetHandleOwners(const QList<uint> &handles)
{
    QList<uint> result;
    for (int i = 0; i < handles.size(); ++i)
    { result.append(0); }
    return result;
}

QList<uint> Channel::GetLocalPendingMembers()
{ return d->local_pending_members; }

QList<uint> Channel::GetMembers()
{ return d->confirmed_members; }

QList<uint> Channel::GetRemotePendingMembers()
{ return d->remote_pending_members; }

uint Channel::GetSelfHandle()
{ return d->connection->GetSelfHandle(); }

// Used to reject Incoming calls.
void Channel::RemoveMembers(const QList<uint> &contacts,
                            const QString & /* message */)
{
    QList<uint> removed_members;
    uint contact;
    foreach (contact, contacts)
    {
        if (d->confirmed_members.contains(contact))
        {
            d->confirmed_members.removeAll(contact);
            removed_members.append(contact);
        }

        if (!removed_members.isEmpty())
        {
            emit MembersChanged(QString(""), QList<uint>(), removed_members,
                                QList<uint>(), QList<uint>(), GetSelfHandle(),
                                0);
        }

        if (d->confirmed_members.size() < 2 &&
            d->state == org::freedesktop::Telepathy::STREAM_STATE_CONNECTED)
        {
            d->state = org::freedesktop::Telepathy::STREAM_STATE_DISCONNECTED;
            qDebug() << "Ch: emitting StreamStateChanged.";
            emit StreamStateChanged(1, d->state);
            qDebug() << "Ch: dialed...";
            Close();
        }
    }
}

bool Channel::hasOpposite(const uint id)
{
    if (d->remote_pending_members.contains(id)) { return true; }
    if (d->confirmed_members.contains(id) && id != GetSelfHandle())
    { return true; }
    return false;
}
