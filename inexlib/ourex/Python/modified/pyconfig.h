#ifndef pyconfig_h
#define pyconfig_h

/*/////////////////////////////////////////*/
/*/// the forever painfull Windows : //////*/
/*/////////////////////////////////////////*/
#ifdef _MSC_VER
#define HAVE_HYPOT 1

#if defined _MSC_VER && _MSC_VER >= 1900 /*G.Barrand*/
#define HAVE_COPYSIGN 1
#endif

#include <io.h>
#define isatty _isatty
#include <process.h> /*for getpid()*/
/* Visual 2015 : to avoid clash with asdl.h that has line :
     typedef enum {false, true} bool;
*/
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif
#ifdef bool
#undef bool
#endif
typedef int pid_t;
#include <direct.h> /*for getcwd()*/
/*
#define HAVE_STRFTIME
#define DONT_HAVE_SIG_ALARM
#define DONT_HAVE_SIG_PAUSE
#define WORD_BIT 32
*/
#ifdef _WIN64
#define MS_WIN64
/*#define LONG_BIT 32*/
#else
#define MS_WIN32
#endif
#define MS_WINDOWS
#define NT_THREADS
/*
#ifndef NETSCAPE_PI
#define USE_SOCKET
#endif
*/

#define HAVE_LARGEFILE_SUPPORT
#if defined(MS_WIN64)
#define PLATFORM "win32"
#define SIZEOF_VOID_P 8
#define SIZEOF_TIME_T 8
#define SIZEOF_OFF_T 4
#define SIZEOF_FPOS_T 8
#define SIZEOF_HKEY 8
#define SIZEOF_SIZE_T 8
#define SIZEOF_LONG 4   /*it is on my Windows-8/64 bits machine*/
#elif defined(MS_WIN32)
#define PLATFORM "win32"
#define SIZEOF_VOID_P 4
#define SIZEOF_OFF_T 4
#define SIZEOF_FPOS_T 8
#define SIZEOF_HKEY 4
#define SIZEOF_SIZE_T 4
#define SIZEOF_LONG 4
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define SIZEOF_TIME_T 8
#else
#define SIZEOF_TIME_T 4
#endif
#endif

/*/////////////////////////////////////////*/
/*/// the forever nice UNIX : /////////////*/
/*/////////////////////////////////////////*/
#else
#define HAVE_UNISTD_H 1
#define HAVE_UTIME_H 1
#define HAVE_DIRENT_H 1
#define HAVE_DLFCN_H 1
#define HAVE_STDINT_H 1
#define HAVE_SYS_WAIT_H 1

#if defined(_LP64)
#define SIZEOF_VOID_P 8
#define SIZEOF_LONG 8
#define SIZEOF_SIZE_T 8
#else
#define SIZEOF_VOID_P 4
#define SIZEOF_LONG 4
#define SIZEOF_SIZE_T 4
#define SIZEOF_OFF_T 8
#define SIZEOF_FPOS_T 8
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if defined(__APPLE__)
#if TARGET_OS_IPHONE
#define PLATFORM "ios"
#else
#define PLATFORM "darwin"
#endif
#endif

#if TARGET_OS_IPHONE
#define SIZEOF_OFF_T  8
#define SIZEOF_FPOS_T 8
#endif

#if defined(__linux__)
#define HAVE_SSIZE_T 1
#define PY_FORMAT_SIZE_T "z"
#ifdef ANDROID
#define PLATFORM "android"
#else
#define PLATFORM "linux2"
#endif
#endif

#define HAVE_SYS_STATVFS_H 1
#ifdef ANDROID
#else
#define HAVE_FSTATVFS 1
#define HAVE_STATVFS 1
#endif

#endif
/*/////////////////////////////////////////*/
/*/////////////////////////////////////////*/
/*/////////////////////////////////////////*/

#define HAVE_FCNTL_H 1
#define HAVE_ERRNO_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_WCHAR_H 1
#define HAVE_SIGNAL_H 1

#define HAVE_DLOPEN 1
#define HAVE_STDARG_PROTOTYPES 1
#define HAVE_INT32_T 1
#define HAVE_UINT32_T 1
#define HAVE_UINT64_T 1
#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG 1
#endif

#define WITH_THREAD 1

#define Py_USING_UNICODE 1
#define Py_UNICODE_SIZE 2
#define PY_UNICODE_TYPE unsigned short
#define PY_FORMAT_LONG_LONG "ll"

#define SIZEOF_INT 4
#define SIZEOF_LONG_LONG 8

/*#define HAVE_DYNAMIC_LOADING 1*/

#endif
