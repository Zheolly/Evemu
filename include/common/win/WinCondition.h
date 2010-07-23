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

#ifndef __WIN__WIN_CONDITION_H__INCL__
#define __WIN__WIN_CONDITION_H__INCL__

#include "threading/Mutex.h"
#include "win/WinCriticalSection.h"
#incldue "win/WinEvent.h"

/**
 * @brief Implementation of condition variable for Windows.
 *
 * @author Bloody.Rabbit
 */
class WinCondition
{
public:
    /// A primary constructor.
    WinCondition();

    /**
     * @brief Signals the condition variable.
     *
     * Signaling the condition makes exactly one
     * waiting thread wake up. If there is none,
     * the signal has no effect.
     *
     * @retval TRUE  Signal succeeded.
     * @retval FALSE Signal failed.
     */
    BOOL Signal();
    /**
     * @brief Broadcasts the condition variable.
     *
     * Broadcasting the condition makes all waiting
     * threads wake up. If there are none, the
     * broadcast has no effect.
     *
     * @retval TRUE  Broadcast succeeded.
     * @retval FALSE Broadcast failed.
     */
    BOOL Broadcast();

    /**
     * @brief Waits on the condition variable.
     *
     * @param[in] criticalSection The critical section this
     *                            condition is bound to.
     * @param[in] timeout         The wait timeout.
     *
     * @return A value returned by @c WaitForSingleObject.
     */
    DWORD Wait( WinCriticalSection& criticalSection, DWORD timeout = INFINITE );

protected:
    /// The event for waiting threads.
    WinEvent mWaitEvent;

    /// Current count of waiting threads.
    SIZE_T mCurrentCount;
    /// Count of threads to free.
    SIZE_T mToFreeCount;

    /// A mutex to protect this whole stuff.
    Mutex mMutex;
};

#endif /* !__WIN__WIN_CONDITION_H__INCL__ */