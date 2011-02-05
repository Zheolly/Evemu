/**
 * @file posix/Fd.h
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

#ifndef __COMMON__POSIX__FD_H__INCL__
#define __COMMON__POSIX__FD_H__INCL__

namespace common
{
    /**
     * @brief Wrappers around POSIX API.
     */
    namespace posix
    {
        /**
         * @brief A file descriptor.
         *
         * @author Bloody.Rabbit
         */
        class Fd
        {
        public:
            /**
             * @brief A primary constructor.
             *
             * @param[in] fd The descriptor
             */
            Fd( int fd = -1 );
            /**
             * @brief A destructor, closes the descriptor.
             */
            ~Fd();

            /**
             * @brief Checks if the descriptor is valid.
             *
             * @return true  The descriptor is valid.
             * @return false The descriptor is invalid.
             */
            bool isValid() const { return -1 != mFd; }
            /**
             * @brief Conventient conversion to boolean.
             *
             * @return A value returned by isValid.
             */
            operator bool() const { return isValid(); }

            /**
             * @brief Closes the descriptor.
             *
             * This method does not fail is the descriptor
             * is already closed.
             *
             * @return An error code.
             */
            int Close();

        protected:
            /// The file descriptor.
            int mFd;
        };

        /**
         * @brief A readable file descriptor.
         *
         * @author Bloody.Rabbit
         */
        class ReadableFd
        : virtual public Fd,
          public stream::Input< void >
        {
        public:
            /**
             * @brief A primary constructor.
             *
             * @param[in] fd The descriptor.
             */
            ReadableFd( int fd = -1 );

            /**
             * @brief Reads data from the file.
             *
             * @param[out] data      Where to store the data.
             * @param[out] bytesRead Where to store a number of read bytes.
             *
             * @return An error code.
             */
            stream::Error Read( util::Data& data, size_t* bytesRead = NULL );
        };

        /**
         * @brief A writable file descriptor.
         *
         * @author Bloody.Rabbit
         */
        class WritableFd
        : virtual public Fd,
          public stream::Output< void >
        {
        public:
            /**
             * @brief A primary constructor.
             *
             * @param[in] fd The descriptor.
             */
            WritableFd( int fd = -1 );

            /**
             * @brief Writes data to the file.
             *
             * @param[in]  data         What data to write.
             * @param[out] bytesWritten Where to store a number of written bytes.
             *
             * @return An error code.
             */
            stream::Error Write( const util::Data& data, size_t* bytesWritten = NULL );
        };
    }
}

#endif /* !__COMMON__POSIX__FD_H__INCL__ */
