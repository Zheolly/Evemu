/**
 * @file posix/File.cpp
 *
 * This file is part of EVEmu: EVE Online Server Emulator.<br>
 * Copyright (C) 2006-2011 The EVEmu Team<br>
 * For the latest information visit <i>http://evemu.org</i>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * <i>http://www.gnu.org/copyleft/lesser.txt</i>.
 *
 * @author Bloody.Rabbit
 */

#include "CommonOs.h"

#include "posix/File.h"

using namespace common;
using namespace common::posix;

/*************************************************************************/
/* common::posix::File                                                   */
/*************************************************************************/
int File::Rename( const char* nameOld, const char* nameNew )
{
    if( 0 != ::rename( nameOld, nameNew ) )
        return errno;

    return 0;
}

int File::Remove( const char* name )
{
    if( 0 != ::remove( name ) )
        return errno;

    return 0;
}

int File::Stat( const char* name, struct stat& buf )
{
    if( 0 != ::stat( name, &buf ) )
        return errno;

    return 0;
}

File::File()
: Fd(),
  ReadableFd(),
  WritableFd()
{
}

File::File( const char* name, int flags, mode_t mode )
: Fd(),
  ReadableFd(),
  WritableFd()
{
    int code = Open( name, flags, mode );
    assert( 0 == code );
}

int File::Stat( struct stat& buf ) const
{
    if( 0 != ::fstat( mFd, &buf ) )
        return errno;

    return 0;
}

int File::Open( const char* name, int flags, mode_t mode )
{
    int code = Close();
    if( 0 != code )
        return code;

    mFd = ::open( name, flags, mode );
    if( !isValid() )
        return errno;

    return 0;
}

int File::Seek( off_t offset, int whence, off_t* result )
{
    off_t res = ::lseek( mFd, offset, whence );
    if( 0 > res )
        return errno;

    if( NULL != result )
        *result = res;

    return 0;
}

int File::Sync()
{
    if( 0 != ::fsync( mFd ) )
        return errno;

    return 0;
}