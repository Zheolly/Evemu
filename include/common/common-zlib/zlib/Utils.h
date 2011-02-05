/**
 * @file zlib/Utils.h
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
 * @author Zhur
 */

#ifndef __COMMON__ZLIB__UTILS_H__INCL__
#define __COMMON__ZLIB__UTILS_H__INCL__

namespace common
{
    /**
     * @brief Contains utilities to access zlib functionality.
     */
    namespace zlib
    {
        /// A zlib stream marker.
        extern const uint8 HEADER_BYTE;

        /**
         * @brief Checks whether given data is deflated.
         *
         * @param[in] data Data to be checked.
         *
         * @retval true  Data is deflated.
         * @retval false Data is not deflated.
         */
        bool IsDeflated( const util::Data& data );

        /**
         * @brief Deflates given data.
         *
         * @param[in,out] data Data to be deflated, overwritten by result.
         *
         * @retval true  Deflation ran successfully.
         * @retval false Error occurred during deflation.
         */
        bool DeflateData( util::Buffer& data );
        /**
         * @brief Deflates given data.
         *
         * @param[in]  input  Data to be deflated.
         * @param[out] output Destination of deflated data.
         *
         * @retval true  Deflation ran successfully.
         * @retval false Error occurred during deflation.
         */
        bool DeflateData( const util::Data& input, util::Buffer& output );

        /**
         * @brief Inflates given data.
         *
         * @param[in,out] data Data to be inflated, overwritten by result.
         *
         * @retval true  Inflation ran successfully.
         * @retval false Failed to inflate data.
         */
        bool InflateData( util::Buffer& data );
        /**
         * @brief Inflates given data.
         *
         * One of the key things of the ZLIB stuff is that we 'sometimes'
         * don't know the size of the uncompressed data. My idea is to fix
         * this regarding the first phase of the parsing of the data
         * (the parser) is to go trough a couple of output buffer size.
         *
         * The first buffer size would be 2x the initial buffer size,
         * implying that the compression ratio is about 50%.
         * The second buffer size would be 4x the initial buffer size,
         * implying that the compression ratio is about 75%.
         * The third buffer size would be 8x the initial buffer size
         * implying that the compression ratio is about 87.5%.
         *
         * We would go on in this progress until we find buffer size
         * big enough to hold uncompressed data.
         *
         * This theory is really stupid because there is no way to actually know.
         *
         * @param[in]  input  Data to be inflated.
         * @param[out] output Destination for inflated data.
         *
         * @retval true  Inflation ran successfully.
         * @retval false Failed to inflate data.
         */
        bool InflateData( const util::Data& input, util::Buffer& output );
    }
}

#endif /* !__COMMON__ZLIB__UTILS_H__INCL__ */
