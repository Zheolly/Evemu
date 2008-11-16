/*  EVEmu: EVE Online Server Emulator

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY except by those people which sell it, which
  are required to give you total support for your newly bought product;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
	
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef TYPES_H
#define TYPES_H


#ifndef WIN32
	#include <stdint.h>
#endif


/* Use correct types for x64 platforms, too */
#ifdef WIN32
	typedef signed __int64 int64;
	typedef signed __int32 int32;
	typedef signed __int16 int16;
	typedef signed __int8 int8;

	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;
#else
	typedef int64_t int64;
	typedef int32_t int32;
	typedef int16_t int16;
	typedef int8_t int8;

	typedef uint64_t uint64;
	typedef uint32_t uint32;
	typedef uint16_t uint16;
	typedef uint8_t uint8;

	typedef uint32_t DWORD;
#endif

typedef uint8				byte;

typedef unsigned long		ulong;
typedef unsigned short		ushort;
typedef unsigned char		uchar;

//for printf() compatibility, I don't like this but its needed.
#ifdef WIN32
	#define I64d "%I64d"
	#define I64u "%I64u"
	#define I64x "%I64x"
	#define I64X "%I64X"
#else
	#define I64d "%lld"
	#define I64u "%llu"
	#define I64x "%llx"
	#define I64X "%llX"
#endif

#ifndef __cplusplus
typedef enum { true, false } bool;
#endif

typedef const char Const_char;	//for perl XS

#ifdef WIN32
	#define snprintf	_snprintf
#if _MSC_VER < 1500
	#define vsnprintf	_vsnprintf
#endif
	#define strncasecmp	_strnicmp
	#define strcasecmp  _stricmp
	typedef void ThreadReturnType;
//	#define THREAD_RETURN(x) return;
	#define THREAD_RETURN(x) _endthread(); return; 
#else
	typedef void* ThreadReturnType;
//	typedef int SOCKET;
	#define THREAD_RETURN(x) return(x);
#endif

#define safe_delete(d) if(d) { delete d; d=0; }
#define safe_delete_array(d) if(d) { delete[] d; d=0; }

#define L32(i)	((int32) i)
#define H32(i)	((int32) (i >> 32))
#define L16(i)	((int16) i)
#define H16(i)  ((int16) (i >> 16))

#ifndef WIN32
// More WIN32 compatibility
	//typedef unsigned long DWORD;
	typedef unsigned char BYTE;
	typedef char CHAR;
	typedef unsigned short WORD;
	typedef float FLOAT;
	typedef FLOAT *PFLOAT;
	typedef BYTE *PBYTE,*LPBYTE;
	typedef int *PINT,*LPINT;
	typedef WORD *PWORD,*LPWORD;
	typedef long *LPLONG, LONG;
	typedef DWORD *PDWORD,*LPDWORD;
	typedef int INT;
	typedef unsigned int UINT,*PUINT,*LPUINT;
#endif


#ifdef WIN32
#define DLLFUNC extern "C" __declspec(dllexport)
#else
#define DLLFUNC extern "C"
#endif



#endif