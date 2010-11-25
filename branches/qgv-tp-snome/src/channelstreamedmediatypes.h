/*
 * QtTelepathy, the Tapioca Qt4 Telepathy Client Library
 * Copyright (C) 2006 by Tobias Hunger <tobias.hunger@basyskom.de>
 * Copyright (C) 2006 by INdT
 *  @author Andre Moreira Magalhaes <andre.magalhaes@indt.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef QTTELEPATHY_CHANNELSTREAMEDMEDIATYPES_H
#define QTTELEPATHY_CHANNELSTREAMEDMEDIATYPES_H

#include <QtCore/QList>
#include <QtDBus/QDBusArgument>
#include <basetypes.h>

namespace org {
namespace freedesktop {
namespace Telepathy {

enum StreamType {
    STREAM_TYPE_AUDIO = 0,
    STREAM_TYPE_VIDEO = 1
};

enum StreamDirection {
    STREAM_DIRECTION_NONE = 0,
    STREAM_DIRECTION_SEND = 1,
    STREAM_DIRECTION_RECEIVE = 2,
    STREAM_DIRECTION_BIDIRECTIONAL = 3
};

enum StreamState {
    STREAM_STATE_DISCONNECTED = 0,
    STREAM_STATE_CONNECTING = 1,
    STREAM_STATE_CONNECTED = 2
};

class StreamInfo
{
public:
    uint id;
    uint contactHandle;
    quint32 type;
    quint32 state;
    quint32 direction;
    quint32 pendingFlags;
};
typedef QList<StreamInfo> StreamInfoList;
typedef QList<uint> StreamTypeList;

} // namespace Telepathy
} // namespace freedesktop
} // namespace org

Q_DECLARE_METATYPE(org::freedesktop::Telepathy::StreamInfo)
Q_DECLARE_METATYPE(org::freedesktop::Telepathy::StreamInfoList)

const QDBusArgument &operator>>(const QDBusArgument &argument, org::freedesktop::Telepathy::StreamInfo &info);
QDBusArgument &operator<<(QDBusArgument &argument, const org::freedesktop::Telepathy::StreamInfo &info);

#endif

