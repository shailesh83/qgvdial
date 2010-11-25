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
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SNOM_CHANNEL_H_
#define _SNOM_CHANNEL_H_

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusContext>

#include "channelstreamedmediatypes.h"

class ChannelPrivate;
class Connection;

/**
 *
 * Telepathy Group interface:
 *
 * Incoming call:
 *
 * Initial state (Ringing):
 *     Handles:
 *        selfhandle in local pending list, remotehandle
 *        is in the confirmed list.
 *     Possible actions:
 *        Disconnect from remote: Channel closes -> Disconnected state.
 *        Disconnect: Close the channel -> Disconnected state
 *        Connect: AddMember on selfhandle (moves selfhandle into
 *                 confirmed list) -> Connected state.
 *
 * Connected state:
 *     Handles:
 *        selfhandle and remote handle in confirmed list
 *     Possible actions:
 *        Disconnect from remote: Channel closes -> Disconnected state.
 *        Disconnect: Close channel -> Disconnected state.
 *
 * Disconnected state:
 *     Channel is closed, no handles.
 *
 *
 * Outgoing call:
 *
 * Initial state:
 *     Handles:
 *        selfhandle in confirmed list, remotehandle in remote pending list.
 *     Possible actions:
 *        Disconnect from remote: Channel closes -> Disconnected state.
 *        Disconnect: Close channel -> Disconnected state
 *        Connect: Remote side accepts the connection -> Connected state.
 */

class Channel : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit Channel(Connection *, QObject * parent = 0);
    ~Channel();

    bool outgoingChannel();

    QString name() const;
    QString serviceName() const;
    QDBusObjectPath objectPath() const;

    bool registerObject();
    void unregisterObject();

    void setStreamState(const org::freedesktop::Telepathy::StreamState);

    void incomingChannel(const uint);
    void outgoingChannel(const uint);

    bool connectOutgoingChannel();

    bool hasOpposite(const uint);

public slots:
    // Channel:
    void Close();
    QString GetChannelType() const;
    uint GetHandle(uint &id) const;
    QStringList GetInterfaces() const;

    // Channel.StreamedMedia:
    org::freedesktop::Telepathy::StreamInfoList ListStreams();
    void RemoveStreams(org::freedesktop::Telepathy::StreamTypeList streams);
    void RequestStreamDirection(uint stream_id, uint stream_direction);
    org::freedesktop::Telepathy::StreamInfoList RequestStreams(uint contact_handle, org::freedesktop::Telepathy::StreamTypeList types);

    // Channel.Interface.DTMF
    void StartTone(uint stream_id, uchar event);
    void StopTone(uint stream_id);

    // Channel.Interface.Group
    void AddMembers(const QList<uint> &contacts, const QString &message);
    QList<uint> GetAllMembers(QList<uint> &local_pending, QList<uint> &remote_pending);
    uint GetGroupFlags();
    QList<uint> GetHandleOwners(const QList<uint> &handles);
    QList<uint> GetLocalPendingMembers();
    QList<uint> GetMembers();
    QList<uint> GetRemotePendingMembers();
    uint GetSelfHandle();
    void RemoveMembers(const QList<uint> &contacts, const QString &message);

signals:
    // Channel:
    void Closed();

    // Channel.StreamedMedia:
    void StreamAdded(uint stream_id, uint contact_handle, uint stream_type);
    void StreamDirectionChanged(uint stream_id, uint stream_direction, uint pending_flags);
    void StreamError(uint stream_id, uint errno, const QString &message);
    void StreamRemoved(uint stream_id);
    void StreamStateChanged(uint stream_id, uint stream_state);

    // Channel.Interface.DTMF
    // none

    // Channel.Interface.Group
    void GroupFlagsChanged(uint added, uint removed);
    void MembersChanged(const QString &message,
                        const QList<uint> &added,
                        const QList<uint> &removed,
                        const QList<uint> &local_pending,
                        const QList<uint> &remote_pending,
                        uint actor,
                        uint reason);

private:
    Channel(const Channel &); // no impl.

    ChannelPrivate * const d;
};

#endif
