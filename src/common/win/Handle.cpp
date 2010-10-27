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

#include "win/Handle.h"

/*************************************************************************/
/* Win::Handle                                                           */
/*************************************************************************/
Win::Handle::Handle( HANDLE handle )
: mHandle( handle )
{
}

Win::Handle::Handle( const Win::Handle& oth )
: mHandle( INVALID_HANDLE_VALUE )
{
    /* Actual handle duplication is quite complex,
       so use our copy operator. */
    *this = oth;
}

Win::Handle::~Handle()
{
    BOOL success = Close();
    assert( TRUE == success );
}

BOOL Win::Handle::Close()
{
    if( TRUE == isValid() )
    {
        if( TRUE != ::CloseHandle( mHandle ) )
            return FALSE;
    }

    mHandle = INVALID_HANDLE_VALUE;
    return TRUE;
}

Win::Handle& Win::Handle::operator=( const Win::Handle& oth )
{
    BOOL success = Close();
    assert( TRUE == success );

    if( TRUE == oth.isValid() )
    {
        success = ::DuplicateHandle( ::GetCurrentProcess(), oth.mHandle,
                                     ::GetCurrentProcess(), &mHandle,
                                     0, FALSE, DUPLICATE_SAME_ACCESS );
        assert( TRUE == success );
    }

    return *this;
}

/*************************************************************************/
/* Win::WaitableHandle                                                   */
/*************************************************************************/
Win::WaitableHandle::WaitableHandle( HANDLE handle )
: Win::Handle( handle )
{
}

DWORD Win::WaitableHandle::Wait( const Time::Msec& timeout ) const
{
    return ::WaitForSingleObject( mHandle, static_cast< DWORD >( timeout.count() ) );
}