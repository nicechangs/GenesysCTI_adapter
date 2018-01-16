//
//  iType.h
//  iCore
//
//  Created by 류 관중 on 12. 5. 14..
//  Copyright (c) 2012년 TonyAppz. All rights reserved.
//

/**
 * @file    iType.h
 * @brief   iCORE용 Variable Type 정의 및 OS 별 Define
 *
 * @version v0.1
 * @see     None
 * @date    2012.5.14
 * @warning 아래 Definition들을 사용하려고 한다면
 *          Definition을 사용하는곳 위에 iType.h를 include 하여야 함
 */

#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>

// Compiler Check
#if (!defined (__GNUC__) && !defined (_MSC_VER) && !defined (__INTEL_COMPILER))
#error "Support GCC, MS Visual Studio, Xcode"
#endif


#if (defined (__ia64__) || defined (__IA64__) || defined (_M_IA64) || defined (__LP64__) || defined(_WIN64) || defined(__amd64) || defined(__amd64__))
#define N64BITS
#else
#define N32BITS
#endif


#if (defined (linux) || defined (__linux))
# define IUNIX
# define ILINUX
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>


#elif (defined (_WIN32) || defined (_WIN64))
# define IWINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>


#elif (defined (__APPLE__))
# define IUNIX
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
# define IPHONEOS
#elif TARGET_OS_MAC
# define IMACOSX
#else
#error "Cannot decide the target operating systems. !!!"
#endif
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>


#elif (defined (sun) || defined(__sun))
# define IUNIX
# define ISOLARIS
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>


#else
#error "Cannot decide the target operating systems. !!!"
#endif

typedef signed char				int8;
typedef unsigned char			uint8;
typedef unsigned char			uchar;
typedef signed short			int16;
typedef unsigned short			uint16;
typedef signed int				int32;
typedef unsigned int			uint32;


/******************************************
 * ILINUX Type and Macro Definitions
 ******************************************/
#if defined ILINUX
//#warning "ILINUX"
typedef signed long long		int64;
typedef unsigned long long		uint64;
typedef socklen_t				iSockLen_t;
typedef int32					iSem_t;
typedef int32					iShm_t;
typedef key_t 					iKey_t;
typedef pthread_t				iThread_t;
typedef pthread_cond_t			iEvent_t;
typedef pthread_mutex_t			iMutex_t;
typedef pthread_rwlock_t		iRwLock_t;
typedef pid_t					iPid_t;
typedef sa_family_t				iFamily_t;
typedef unsigned long			dword;

#define IEOLTYPE				("\n")
#define ITIMEOUT				ETIMEDOUT
#define ILIBAPI


/******************************************
 * IWINDOWS Type and Macro Definitions
 ******************************************/
#elif defined IWINDOWS
//#warning "IWINDOWS"
typedef signed __int64			int64;
typedef unsigned __int64		uint64;
typedef int32					iSockLen_t;
typedef HANDLE					iSem_t;
typedef HANDLE					iShm_t;
typedef int32 					iKey_t;
typedef HANDLE					iThread_t;
typedef HANDLE					iEvent_t;
typedef CRITICAL_SECTION		iMutex_t;
typedef CRITICAL_SECTION		iRwLock_t;
typedef DWORD					iPid_t;
typedef int16					iFamily_t;
typedef unsigned long			dword;

typedef int32					socklen_t;		///< Deprecate
typedef int32					pthread_t;		///< Deprecate
typedef uint32					pid_t;			///< Deprecate

#define IEOLTYPE				("\n")
#define ITIMEOUT				WAIT_TIMEOUT
#define ILIBAPI
#define snprintf				_snprintf
#define strncasecmp				_strnicmp


/******************************************
 * IMACOSX Type and Macro Definitions
 ******************************************/
#elif defined IMACOSX
//#warning "IMACOSX"
typedef signed long long		int64;
typedef unsigned long long		uint64;
typedef socklen_t				iSockLen_t;
typedef int32					iSem_t;
typedef int32					iShm_t;
typedef key_t 					iKey_t;
typedef pthread_t				iThread_t;
typedef pthread_cond_t			iEvent_t;
typedef pthread_mutex_t			iMutex_t;
typedef pthread_rwlock_t		iRwLock_t;
typedef pid_t					iPid_t;
typedef sa_family_t				iFamily_t;
typedef unsigned long			dword;

#define IEOLTYPE				("\n")
#define ITIMEOUT				ETIMEDOUT
#define ILIBAPI


/******************************************
 * IPHONEOS Type and Macro Definitions
 ******************************************/
#elif defined IPHONEOS
//#warning "IPHONEOS"
typedef signed long long		int64;
typedef unsigned long long		uint64;
typedef socklen_t				iSockLen_t;
typedef int32					iSem_t;
typedef int32					iShm_t;
typedef key_t 					iKey_t;
typedef pthread_t				iThread_t;
typedef pthread_cond_t			iEvent_t;
typedef pthread_mutex_t			iMutex_t;
typedef pthread_rwlock_t		iRwLock_t;
typedef pid_t					iPid_t;
typedef sa_family_t				iFamily_t;
typedef unsigned long			dword;

#define IEOLTYPE				("\n")
#define ITIMEOUT				ETIMEDOUT
#define ILIBAPI


/******************************************
 * ISOLARIS Type and Macro Definitions
 ******************************************/
#elif defined ISOLARIS
//#warning "ISOLARIS"
typedef signed long long		int64;
typedef unsigned long long		uint64;
typedef socklen_t				iSockLen_t;
typedef int32					iSem_t;
typedef int32					iShm_t;
typedef key_t 					iKey_t;
typedef pthread_t				iThread_t;
typedef pthread_cond_t			iEvent_t;
typedef pthread_mutex_t			iMutex_t;
typedef pthread_rwlock_t		iRwLock_t;
typedef pid_t					iPid_t;
typedef sa_family_t				iFamily_t;
typedef unsigned long			dword;

#define IEOLTYPE				("\n")
#define ITIMEOUT				ETIMEDOUT
#define ILIBAPI


#else
#error "Cannot decide the target operating systems. !!!"
#endif



#define NEOLTYPE				IEOLTYPE

#define MAXLEN_PATH				(260)
#define MAXLEN_HEXDUMP			(512)

#define MAXLEN_C16				(16)
#define MAXLEN_C32				(32)
#define MAXLEN_C64				(64)
#define MAXLEN_C128				(128)
#define MAXLEN_C256				(256)
#define MAXLEN_C512				(512)
#define MAXLEN_LINE				(1024)
#define MAXLEN_LINE4			(4*MAXLEN_LINE)
#define MAXLEN_LINE16			(16*MAXLEN_LINE)
#define MAXLEN_LINE32			(32*MAXLEN_LINE)
#define MAXLEN_LINE64			(64*MAXLEN_LINE)
#define MAXLEN_LINE128			(128*MAXLEN_LINE)
#define MAXLEN_LINE256			(256*MAXLEN_LINE)
#define MAXLEN_LINE512			(512*MAXLEN_LINE)
#define MAXLEN_LINE1M			(MAXLEN_LINE*MAXLEN_LINE)
#define MAXLEN_LINE2M			(2*MAXLEN_LINE*MAXLEN_LINE)
#define MAXLEN_LINE4M			(4*MAXLEN_LINE*MAXLEN_LINE)

#define STRLEN_IPv4				(16)
#define BINLEN_IPv4				(4)
#define STRLEN_IPv6				(64)
#define BINLEN_IPv6				(16)

#define MAXLEN_IPADDR4			(STRLEN_IPv4)
#define MAXLEN_IPADDR6			(STRLEN_IPv6)
#define MAXLEN_IPADDR			(MAXLEN_IPADDR6)
#define MAXLEN_MACADDR			(24)
#define MAXLEN_DATE				(16)	// YYYYMMDD
#define MAXLEN_TIME				(8)		// hhmmss
#define MAXLEN_DATETIME			(MAXLEN_DATE+MAXLEN_TIME)

#define SLOCAL_HOST4			("127.0.0.1")
#define SLOCAL_HOST6			("::1")

#define IMAX(x, y)				((x)>(y)?(x):(y))
#define IMIN(x, y)				((x)<(y)?(x):(y))
#define IMAKEWORD(h, l)			((h)<<16|((l)&0xffff))
#define IHIWORD(v)				((v)>>16)
#define ILOWORD(v)				((v)&0xffff)
#define IMKFOURCC(n)			(((n&0x000000ff)<<24)|((n&0x0000ff00)<<8)|((n&0x00ff0000)>>8)|((n&0xff000000)>>24))
#define IMKTWOCC(n)				(((n&0x00ff)<<8)|((n&0xff00)>>8))

#define IALIGN(n, b)			((((n)+(b)-1)/(b))*(b))
#define IALIGN4(n)				IALIGN(n, 4)
#define IALIGN32(n)				IALIGN(n, 32)

#define IROUND(x, y)			(((x)+(y-1))&~(y-1))
#define IROUND4(x)				(IROUND(x, 4))

#define IABS(x)					((x)>0?(x):-(x))

#define IDELPTR(x)				if(x){delete(x);(x)=NULL;}
#define IDELARR(x)				if(x){delete[](x);(x)=NULL;}
#define ZEROARR(x)				memset((x),0x00,sizeof(x))
#define ZEROCLS(x)				memset(&(x),0x00,sizeof(x))

#define ONESEC					(1000)				// milisec
#define TENSEC					(ONESEC*10)
#define ONEMIN					(ONESEC*60)
#define ONEHOUR					(ONEMIN*60)
#define ONEDAY					(ONEHOUR*24)
#define ONEWEEK					(ONEDAY*7)

#define MIN2SEC					(60)
#define HOUR2SEC				(60*MIN2SEC)
#define DAY2SEC					(24*HOUR2SEC)
#define WEEK2SEC				(7*DAY2SEC)

#define ILIB_INFINITE			(0xffffffff)
#define ILIB_NULLSTR			("")
#define ILIB_NULLCHAR			('\0')

#define IMAX_INT8				(0x7f)
#define IMAX_INT16				(0x7fff)
#define IMAX_INT32				(0x7fffffff)
#define IMAX_INT64				(0x7fffffffffffffff)
#define IMAX_UINT8				(0xff)
#define IMAX_UINT16				(0xffff)
#define IMAX_UINT32				(0xffffffff)
#define IMAX_UINT64				(0xffffffffffffffff)


/******************************************
 * iCore Library Type Definitions
 ******************************************/

typedef void *					NPVOID;

typedef char *					NPSTR;
typedef const char *			NPCSTR;
typedef char const *			NPSTRC;
typedef const char * const		NPCSTRC;

typedef uchar *					UPSTR;
typedef const uchar *			UPCSTR;
typedef uchar const *			UPSTRC;
typedef const uchar * const		UPCSTRC;

typedef uchar					iBit8;
typedef uint16					iBit16;
typedef uint32					iBit32;
typedef uint64					iBit64;
typedef uchar					iByte;
typedef int32					iFd_t;
typedef iFd_t					iFd;			///< Will be DELETE
typedef int64					iSize_t;

typedef NPVOID					NPos;
#define NBEFORE_STARTPOS		((NPos)-1L)


#ifdef __cplusplus
	#ifndef NULL
		#define NULL			(0)
	#endif
#else
	#ifndef NULL
		#define NULL			((NPVOID)0)
	#endif
	typedef uchar				bool;
	#define false				(0)
	#define true				(!false)
#endif


/**
 * for Coding Style
 */

#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
	private:\
	TypeName(const TypeName&) {};\
	void operator =(const TypeName&) {}

#ifdef IUNIX
// for ANSI Color
#define ARED					"\e[31m"
#define AGREEN					"\e[32m"
#define AYELOW					"\e[33m"
#define ABLUE					"\e[34m"
#define APURPLE					"\e[35m"
#define ACYAN					"\e[36m"
#define AWHITE					"\e[37m"

#define ABRED					"\e[41m"
#define ABGREN					"\e[42m"
#define ABYELW					"\e[43m"
#define ABBLUE					"\e[44m"
#define ABPURP					"\e[45m"
#define ABCYAN					"\e[46m"
#define ABWHIT					"\e[47m"

#define ABOLD					"\e[1m"
#define AITLIC					"\e[3m"
#define AULINE					"\e[4m"
#define ABLINK					"\e[5m"
#define AFBLNK					"\e[6m"

#define ANONE					"\e[0m"
#else
#define ARED					""
#define AGREEN					""
#define AYELOW					""
#define ABLUE					""
#define APURPLE					""
#define ACYAN					""
#define AWHITE					""

#define ABRED					""
#define ABGREN					""
#define ABYELW					""
#define ABBLUE					""
#define ABPURP					""
#define ABCYAN					""
#define ABWHIT					""

#define ABOLD					""
#define AITLIC					""
#define AULINE					""
#define ABLINK					""
#define AFBLNK					""

#define ANONE					""
#endif
/*
 [0m  : 모든 색과 스타일 초기화
 [1m  : 굵게(bold) / 밝게
 [3m  : 이탤릭체(italic)
 [4m  : 밑줄(underline)
 [7m  : 반전(글자색/배경색을 거꾸로)
 [9m  : 가로줄 치기
 [22m : 굵게(bold) 제거
 [23m : 이탤릭체(italic)제거
 [24m : 밑줄(underline)제거
 [27m : 반전 제거
 [29m : 가로줄 제거
 [30m : 글자색:검정
 [31m : 글자색:빨강
 [32m : 글자색:초록
 [33m : 글자색:노랑
 [34m : 글자색:파랑
 [35m : 글자색:마젠트(분홍)
 [36m : 글자색:시안(청록)
 [37m : 글자색:백색
 [39m : 글자색으로 기본값으로
 [40m : 바탕색:흑색
 [41m : 바탕색:적색
 [42m : 바탕색:녹색
 [43m : 바탕색:황색
 [44m : 바탕색:청색
 [45m : 바탕색:분홍색
 [46m : 바탕색:청록색
 [47m : 바탕색:흰색
 [49m :바탕색을 기본값으로
 */


#if defined IWINDOWS
#define DO_PRAGMA(x) __pragma (#x)
#else
#define DO_PRAGMA(x) _Pragma (#x)
#endif

#define TODO(x) DO_PRAGMA(message (ABBLUE "TODO - " #x ANONE))
#define WARN(x) DO_PRAGMA(message (ABRED "WARN - " #x ANONE))
#define NOTE(x) DO_PRAGMA(message (AGREEN "NOTE - " #x ANONE))


#if defined ISOLARIS
#define PRAGMA_PACK_BEGIN(x)	DO_PRAGMA(pack(x))
#define PRAGMA_PACK_END			DO_PRAGMA(pack())
#else
#define PRAGMA_PACK_BEGIN(x)	DO_PRAGMA(pack(push,x))
#define PRAGMA_PACK_END			DO_PRAGMA(pack(pop))
#endif

#ifdef __GNUC__
#define IDEPRECATED				__attribute__((deprecated))
#elif defined(_MSC_VER)
#define IDEPRECATED				__declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define IDEPRECATED
#endif







