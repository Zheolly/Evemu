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
	Author:		Zhur
*/


#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include "types.h"
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>


#ifndef ERRBUF_SIZE
#define ERRBUF_SIZE		1024
#endif

//////////////////////////////////////////////////////////////////////
//
//  MakeUpperString
//   i     : source - allocated null-terminated string
//   return: pointer to static buffer with the target string
const char *MakeUpperString(const char *source);
const char *MakeLowerString(const char *source);
//////////////////////////////////////////////////////////////////////
//
//  MakeUpperString
//   i : source - allocated null-terminated string
//   io: target - allocated buffer, at least of size strlen(source)+1
void MakeUpperString(const char *source, char *target);
void MakeLowerString(const char *source, char *target);


#if WIN32

/**
 * @note copied from: http://linux.die.net/man/3/vasprintf
 * The functions asprintf() and vasprintf() are analogues of sprintf() and
 * vsprintf(), except that they allocate a string large enough to hold the
 * output including the terminating null byte, and return a pointer to it
 * via the first parameter. This pointer should be passed to free(3) to
 * release the allocated storage when it is no longer needed.
 *
 * @return When successful, these functions return the number of bytes printed,
 * just like sprintf(3). If memory allocation wasn't possible, or some other
 * error occurs, these functions will return -1, and the contents of strp is undefined.
 * @todo check http://www.koders.com/c/fidA509CB3A8622E36F7CE0851560975BE02C89E9C7.aspx
 */
int	asnprintf(char** strp, const char* fmt, ...);
int	vasprintf(char** strp, const char* fmt, va_list ap);
#endif

int32	AppendAnyLenString(char** ret, int32* bufsize, int32* strlen, const char* format, ...);
int32	hextoi(char* num);
int64	hextoi64(char* num);
bool	atobool(char* iBool);
int32	filesize(FILE* fp);
int32	ResolveIP(const char* hostname, char* errbuf = 0);
//bool	ParseAddress(const char* iAddress, int32* oIP, int16* oPort, char* errbuf = 0);
void	CoutTimestamp(bool ms = true);
char*	strn0cpy(char* dest, const char* source, int32 size);
		// return value =true if entire string(source) fit, false if it was truncated
bool	strn0cpyt(char* dest, const char* source, int32 size);
int	MakeRandomInt(int low, int high);
double	MakeRandomFloat(double low, double high);



#define _ITOA_BUFLEN	25
const char *itoa(int num);	//not thread safe
#ifndef WIN32
const char *itoa(int num, char* a,int b);
#endif

#ifdef WIN32
class InitWinsock {
public:
	InitWinsock();
	~InitWinsock();
};
#endif

#endif
