// platform.h: 
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
//	basic types definition
#if !defined(__NO_BASIC_TYPES)

typedef int					BOOL;
typedef unsigned char		U8;
typedef char				I8;
typedef unsigned short		U16;
typedef short				I16;
typedef unsigned int		U32;
typedef int					I32;
typedef unsigned long long	U64;
typedef long long			I64;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

/* import c99 types */
#if !defined(__NO_SUPORT_C99)
#include <stdint.h>
#include <stdbool.h>

#endif //!define(__NO_SUPORT_C99)

#endif //!defined(NO_BASIC_TYPES)

////////////////////////////////////////////////////////////////////////////////
//	platform types definition

#if defined(WIN32)   
#else
#define strcmpi  strcasecmp
#endif

//#if defined(_GCC_COMPILER_)
//#define ALIGNED(n) __attribute__((align(n)))
//#endif //_GCC_COMPILER_

#if !defined(NO_PLATFORM_TYPES)

typedef void*			LPVOID;
typedef void*			PVOID;
typedef long			LONG;
typedef unsigned long	DWORD;
typedef DWORD*			PDWORD;
typedef DWORD*			LPDWORD;
typedef unsigned short	WORD;
typedef WORD*			PWORD;
typedef WORD*			LPWORD;
typedef U8				BYTE;
typedef U8				UCHAR;
typedef U8*				LPBYTE;
typedef U8*				PUCHAR;
typedef I8*				LPSTR;

#endif //!defined(NO_PLATFORM_TYPES)
////////////////////////////////////////////////////////////////////////////////
//	system operations
const char* os_get_env(const char *name);
BOOL os_set_env(const char *name, const char *value);
BOOL os_remove_env(const char *name);

BOOL os_sleep(U32 nMS);

U32	os_random(U32 range);

typedef void (*psignal_proc_t)(int sig);
void os_set_signal_handler(int sig, psignal_proc_t on_signal_f);

#ifdef __cplusplus
}

////////////////////////////////////////////////////////////////////////////////
//	C++ STL includings

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif //defined(WIN32)

#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>


#include <fstream>
#include <iostream>

using namespace std;


#define LIST_ITERATOR_TYPE(type) _List_iterator<type>


////////////////////////////////////////////////////////////////////////////////
//	utility functions & classes



#endif //__cplusplus


#endif // _PLATFORM_H_


