#ifndef CFUNIX_H
#define CFUNIX_H

/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/

/*G.Barrand : the below is used only in a dump. */
#define CANONICAL_HOST_TYPE "cpp_CANONICAL_HOST_TYPE_not_defined"

/*G.Barrand : use DCMDICTPATH env variable. */
#define DCM_DICT_DEFAULT_PATH "cpp_DCM_DICT_DEFAULT_PATH_not_defined"

/*G.Barrand : to master the handling of DCMDICTPATH. */
#define DONT_LOAD_EXTERNAL_DICTIONARIES

/*
#define CANONICAL_HOST_TYPE "i386-apple-darwin11.4.2"
#define DCM_DICT_DEFAULT_PATH "/usr/local/dcmtk/3.6.0/share/dcmtk/dicom.dic"
*/

/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/

#define PATH_SEPARATOR '/'

#if defined(_LP64) 
#define SIZEOF_LONG   8
#else 
#define SIZEOF_LONG   4
#endif

/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/

#define PACKAGE_BUGREPORT "dicom-bugs@offis.de"
#define PACKAGE_DATE "2011-01-06"
#define PACKAGE_NAME "dcmtk"
#define PACKAGE_STRING "dcmtk 3.6.0"
#define PACKAGE_TARNAME "dcmtk-3.6.0"
#define PACKAGE_VERSION "3.6.0"
#define PACKAGE_VERSION_NUMBER "360"
#define PACKAGE_VERSION_SUFFIX ""

#define STDIO_NAMESPACE ::

#define USE_STD_CXX_INCLUDES 

/*G.Barrand #define WITH_ZLIB */

/*G.Barrand : to avoid a -D in the build system :*/
#define USE_NULL_SAFE_OFSTRING

/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/

#ifdef ANDROID
#else
#ifdef _WIN32
#else
#define HAVE_GETHOSTID 1
#endif
#endif

#ifdef _WIN32
#define ENVIRONMENT_PATH_SEPARATOR ';'
#define HAVE_WINDOWS_H
#define HAVE_WINSOCK_H
#define NO_IOS_BASE_ASSIGN
#else
#define ENVIRONMENT_PATH_SEPARATOR ':'
#define HAVE_STRINGS_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_DIRENT_H 1
#define HAVE_FNMATCH_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_MKSTEMP 1
#define HAVE_PROTOTYPE_STRERROR_R 1
#define HAVE_PROTOTYPE_GETHOSTID 1
#define HAVE_ACCESS 1
#define HAVE_MKTEMP 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_NETDB_H 1
#endif

/* pb on Linux :
#define HAVE_STRLCAT 1
#define HAVE_STRLCPY 1
#define HAVE_LIBC_H 1
#define HAVE_STDARG_H 1
#define HAVE_STDIO_H 1
*/

/* clashes with ones in jpeg.
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
*/

#define HAVE_ACCEPT 1
#define HAVE_ALLOCA_H 1
#define HAVE_ASSERT_H 1
#define HAVE_BCMP 1
#define HAVE_BCOPY 1
#define HAVE_BIND 1
#define HAVE_BZERO 1
#define HAVE_CLASS_TEMPLATE 1
#define HAVE_CONNECT 1
#define HAVE_CONST_CAST 
#define HAVE_CSTDARG 1
#define HAVE_CSTDIO 1
#define HAVE_CTYPE_H 1
#define HAVE_CXX_BOOL 1
#define HAVE_CXX_VOLATILE 
#define HAVE_C_CONST 1
#define HAVE_DECLARATION_SOCKLEN_T 1
#define HAVE_DECLARATION_STD__IOS_BASE__OPENMODE 1
#define HAVE_DECLARATION_STRUCT_UTIMBUF 1
#define HAVE_DYNAMIC_CAST 
#define HAVE_ENAMETOOLONG 
#define HAVE_ERRNO_H 1
#define HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION 1
#define HAVE_FCNTL_H 1
#define HAVE_FINITE 1
#define HAVE_FLOAT_H 1
#define HAVE_FLOCK 1
#define HAVE_FORK 1
#define HAVE_FSEEKO 1
#define HAVE_FSTREAM 1
#define HAVE_FSTREAM_H 1
#define HAVE_FTIME 1
#define HAVE_FUNCTION_TEMPLATE 1
#define HAVE_GETADDRINFO 1
#define HAVE_GETENV 1
#define HAVE_GETEUID 1
#define HAVE_GETGRNAM 1
#define HAVE_GETHOSTBYNAME 1
#define HAVE_GETHOSTNAME 1
#define HAVE_GETLOGIN 1
#define HAVE_GETPID 1
#define HAVE_GETPWNAM 1
#define HAVE_GETSOCKNAME 1
#define HAVE_GETSOCKOPT 1
#define HAVE_GETUID 1
#define HAVE_GMTIME_R 1
#define HAVE_GRP_H 1
#define HAVE_HTONL 1
#define HAVE_HTONS 1
#define HAVE_INDEX 1
#define HAVE_INT64_T 1
#define HAVE_INTTYPES_H 1
#define HAVE_IOMANIP 1
#define HAVE_IOMANIP_H 1
#define HAVE_IOSTREAM 1
#define HAVE_IOSTREAM_H 1
#define HAVE_ISNAN 1
#define HAVE_ISO646_H 1
#define HAVE_LFS_SUPPORT 
#define HAVE_LIMITS_H 1
#define HAVE_LISTEN 1
#define HAVE_LOCALE_H 1
#define HAVE_LOCALTIME_R 1
#define HAVE_LONG_FILE_NAMES 1
#define HAVE_LSTAT 1
#define HAVE_MALLOC_DEBUG 1
#define HAVE_MATH_H 1
#define HAVE_MEMCMP 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMORY_H 1
#define HAVE_MEMSET 1
#define HAVE_NETINET_IN_SYSTM_H 1
#define HAVE_NETINET_TCP_H 1
#define HAVE_NEW 1
#define HAVE_NEW_H 1
#define HAVE_NOTHROW_DELETE 

#define HAVE_NTOHL 1
#define HAVE_NTOHS 1
#define HAVE_POINTER_TYPE_PTHREAD_T 1
#define HAVE_PROTOTYPE_ACCEPT 1
#define HAVE_PROTOTYPE_BIND 1
#define HAVE_PROTOTYPE_BZERO 1
#define HAVE_PROTOTYPE_CONNECT 1
#define HAVE_PROTOTYPE_FINITE 1
#define HAVE_PROTOTYPE_FLOCK 1
#define HAVE_PROTOTYPE_GETHOSTBYNAME 1
#define HAVE_PROTOTYPE_GETHOSTNAME 1
#define HAVE_PROTOTYPE_GETSOCKNAME 1
#define HAVE_PROTOTYPE_GETSOCKOPT 1
#define HAVE_PROTOTYPE_GETTIMEOFDAY 1
#define HAVE_PROTOTYPE_ISNAN 1
#define HAVE_PROTOTYPE_LISTEN 1
#define HAVE_PROTOTYPE_MKSTEMP 1
#define HAVE_PROTOTYPE_MKTEMP 1
#define HAVE_PROTOTYPE_SELECT 1
#define HAVE_PROTOTYPE_SETSOCKOPT 1
#define HAVE_PROTOTYPE_SOCKET 1
#define HAVE_PROTOTYPE_STD__VFPRINTF 1
#define HAVE_PROTOTYPE_STD__VSNPRINTF 1
#define HAVE_PROTOTYPE_STRCASECMP 1
#define HAVE_PROTOTYPE_STRNCASECMP 1
#define HAVE_PROTOTYPE_USLEEP 1
#define HAVE_PROTOTYPE_VSNPRINTF 1
#define HAVE_PROTOTYPE_WAIT3 1
#define HAVE_PROTOTYPE_WAITPID 1
#define HAVE_PTHREAD_H 1
#define HAVE_PWD_H 1
#define HAVE_REINTERPRET_CAST 
#define HAVE_SELECT 1
#define HAVE_SEMAPHORE_H 1
#define HAVE_SETJMP_H 1
#define HAVE_SETSOCKOPT 1
#define HAVE_SIGNAL_H 1
#define HAVE_SLEEP 1
#define HAVE_SOCKET 1
#define HAVE_SSTREAM 1
#ifndef HAVE_STAT /*G.Barrand : to rm warning with Python.h. */
#define HAVE_STAT 1
#endif
#define HAVE_STATIC_CAST 
#define HAVE_STATIC_TEMPLATE_METHOD 1
#define HAVE_STD_NAMESPACE 1
#define HAVE_STD__NOTHROW 
#define HAVE_STRCHR 1
#define HAVE_STRDUP 1
#define HAVE_STRERROR 1
#define HAVE_STRING_H 1
#define HAVE_STRSTR 1
#define HAVE_STRSTREAM 1
#define HAVE_STRTOUL 1
#define HAVE_SYSLOG_H 1
#define HAVE_SYS_ERRNO_H 1
#define HAVE_SYS_FILE_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_RESOURCE_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_TEMPNAM 1
#define HAVE_TIME_H 1
#define HAVE_TMPNAM 1
#define HAVE_TYPENAME 
#define HAVE_UNAME 1
#define HAVE_USLEEP 1
#define HAVE_VPRINTF 1
#define HAVE_VSNPRINTF 1
#define HAVE_WCTYPE_H 1

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
/* pb on iOS with isinf() when validating for AppleStore
   (non-public API usage) */
#else
#ifdef _WIN32
#else
#define HAVE_ISINF 1
#define HAVE_PROTOTYPE_ISINF 1
#endif
#endif

/*/////////////////////////////////////////////////////////////*/
/*/// not used ////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////////////////*/
/*
#define HAVE_LOCKF 1
#define HAVE_RINDEX 1
#define HAVE_STDINT_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_SETUID 1
#define HAVE_UINT64_T 1
#define HAVE_UTIME_H 1
#define HAVE_VLA 1
#define HAVE_WAIT3 1
#define HAVE_WAITPID 1

#define DCMTK_PREFIX "/usr/local/dcmtk/3.6.0"
#define DEFAULT_CONFIGURATION_DIR "/usr/local/dcmtk/3.6.0/etc/dcmtk/"
#define DEFAULT_SUPPORT_DATA_DIR "/usr/local/dcmtk/3.6.0/share/dcmtk/"

#define SIZEOF_CHAR 1
#define SIZEOF_DOUBLE 8
#define SIZEOF_FLOAT 4
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_SHORT 2
#define SIZEOF_VOID_P 8

#define RETSIGTYPE void
#define STDC_HEADERS 1
#define C_INLINE inline

#ifdef HAVE_NO_TYPEDEF_PID_T
typedef int pid_t;
#endif

#ifdef HAVE_NO_TYPEDEF_SIZE_T
typedef unsigned size_t;
#endif

#ifdef HAVE_NO_TYPEDEF_SSIZE_T
typedef long ssize_t;
#endif

#define HAVE_NO_TYPEDEF_UCHAR 1
#ifdef HAVE_NO_TYPEDEF_UCHAR
typedef unsigned char uchar;
#endif

#ifdef HAVE_NO_TYPEDEF_UINT
typedef unsigned int uint;
#endif

#define HAVE_NO_TYPEDEF_ULONG 1
#ifdef HAVE_NO_TYPEDEF_ULONG
typedef unsigned long ulong;
#endif

#ifdef HAVE_NO_TYPEDEF_USHORT
typedef unsigned short ushort;
#endif

*/

#endif
