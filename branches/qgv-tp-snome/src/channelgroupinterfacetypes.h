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

#ifndef QTTELEPATHY_CHANNELIFACEGROUP_H
#define QTTELEPATHY_CHANNELIFACEGROUP_H

namespace org {
namespace freedesktop {
namespace Telepathy {

enum GroupFlag {
    CHANNEL_GROUP_FLAG_NONE                     = 0x00000000,
    CHANNEL_GROUP_FLAG_CAN_ADD                  = 0x00000001,
    CHANNEL_GROUP_FLAG_CAN_REMOVE               = 0x00000002,
    CHANNEL_GROUP_FLAG_CAN_RESCIND              = 0x00000004,
    CHANNEL_GROUP_FLAG_MESSAGE_ADD              = 0x00000008,
    CHANNEL_GROUP_FLAG_MESSAGE_REMOVE           = 0x00000010,
    CHANNEL_GROUP_FLAG_MESSAGE_ACCEPT           = 0x00000020,
    CHANNEL_GROUP_FLAG_MESSAGE_REJECT           = 0x00000040,
    CHANNEL_GROUP_FLAG_MESSAGE_RESCIND          = 0x00000080,
    CHANNEL_GROUP_FLAG_CHANNEL_SPECIFIC_HANDLES = 0x00000100,
    CHANNEL_GROUP_FLAG_ONLY_ONE_GROUP		= 0x00000200,
    CHANNEL_GROUP_FLAG_HANDLE_OWNERS_NOT_AVAILABLE = 0x00000400,
    CHANNEL_GROUP_FLAG_ALL                      = 0xFFFFFFFF
};
Q_DECLARE_FLAGS(GroupFlags, GroupFlag)

} // namespace Telepathy
} // namespace freedesktop
} // namespace org

Q_DECLARE_OPERATORS_FOR_FLAGS(org::freedesktop::Telepathy::GroupFlags)

#endif

