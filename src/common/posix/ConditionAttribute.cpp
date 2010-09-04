/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2008 The EVEmu Team
    For the latest information visit http://evemu.mmoforge.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Bloody.Rabbit
*/

#include "CommonPCH.h"

#include "posix/ConditionAttribute.h"

/*************************************************************************/
/* Posix::Condition::Attribute                                           */
/*************************************************************************/
Posix::Condition::Attribute::Attribute()
{
    int code;

    code = ::pthread_condattr_init( &mAttribute );
    assert( 0 == code );
}

Posix::Condition::Attribute::Attribute( int processShared )
{
    int code;

    code = ::pthread_condattr_init( &mAttribute );
    assert( 0 == code );

    code = SetProcessShared( processShared );
    assert( 0 == code );
}

Posix::Condition::Attribute::~Attribute()
{
    int code;

    code = ::pthread_condattr_destroy( &mAttribute );
    assert( 0 == code );
}

int Posix::Condition::Attribute::GetProcessShared( int* processShared ) const
{
    return ::pthread_condattr_getpshared( &mAttribute, processShared );
}

int Posix::Condition::Attribute::SetProcessShared( int processShared )
{
    return ::pthread_condattr_setpshared( &mAttribute, processShared );
}