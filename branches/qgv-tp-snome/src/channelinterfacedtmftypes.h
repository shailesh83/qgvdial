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

#ifndef QTTELEPATHY_CHANNELTYPEDTMF_H
#define QTTELEPATHY_CHANNELTYPEDTMF_H

#include "basetypes.h"

namespace org {
namespace freedesktop {
namespace Telepathy {

enum DTMF_Event {
    Digit_0 = 0,
    Digit_1 = 1,
    Digit_2 = 2,
    Digit_3 = 3,
    Digit_4 = 4,
    Digit_5 = 5,
    Digit_6 = 6,
    Digit_7 = 7,
    Digit_8 = 8,
    Digit_9 = 9,
    Asterisk = 10,
    Hash = 11,
    Letter_A = 12,
    Letter_B = 13,
    Letter_C = 14,
    Letter_D = 15
};

} // namespace Telepathy
} // namespace freedesktop
} // namespace org

#endif

