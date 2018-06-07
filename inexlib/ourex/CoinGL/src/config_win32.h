#ifndef COIN_INTERNAL
#error this is a private header file
#endif

#include <unconfig.h>
#include "setup.h"

#define COIN_CONFIGURE_BUILD I686_PC_WINDOWS_XP
#define COIN_CONFIGURE_HOST I686_PC_WINDOWS_XP
#define COIN_CONFIGURE_TARGET I686_PC_WINDOWS_XP

#define COIN_INT16_T int16_t
#define COIN_INT32_T int32_t
#define COIN_INT64_T int64_t
#define COIN_INT8_T int8_t
#define COIN_UINT16_T uint16_t
#define COIN_UINT32_T uint32_t
#define COIN_UINT64_T uint64_t
#define COIN_UINT8_T uint8_t

#define COIN_MAJOR_VERSION 2
#define COIN_MICRO_VERSION 4
#define COIN_MINOR_VERSION 4
#define COIN_VERSION "2.4.4"

#define HAVE_HASH_QUOTING 1
#define HAVE_SINGLEPRECISION_MATH 1
#define HAVE_VAR___func_ 1

#define HAVE_WIN32_API 1
#define HAVE_WINDOWS_H 1
#define HAVE_WINSOCK2_H 1
#define HAVE__SNPRINTF 1
#define HAVE__VSNPRINTF 1

#define HAVE_FSTAT 1
#define HAVE_SYS_STAT_H 1

#define X_DISPLAY_MISSING 1

#define HAVE_THREADS
#define USE_W32THREAD 1

/* Fundamental for time sensors (animation) : */
#define HAVE__FTIME
#define HAVE_TIME_H 1
#define HAVE_SYS_TIMEB_H 1
#define HAVE_IO_H 1
                                                      
#ifdef OUREX_COINGL_INLIB_GL
#else
#define HAVE_WGL 1
#endif
#define HAVE_GL_GL_H 1

#define PACKAGE "Coin"
#define PACKAGE_BUGREPORT "coin-support@coin3d.org"
#define PACKAGE_NAME "Coin"
#define PACKAGE_STRING "Coin 2.4.4"
#define PACKAGE_TARNAME "coin"
#define PACKAGE_VERSION "2.4.4"
#define VERSION "2.4.4"

#define GLUWRAPPER_ASSUME_GLU 1

/* NOTE : We use ourex/zlib. ourex/HEPVis must be consistent with what is done here.*/
#define HAVE_ZLIB 1
