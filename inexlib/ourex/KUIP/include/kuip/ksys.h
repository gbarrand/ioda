#ifndef kuip_ksys_h
#define kuip_ksys_h

/*G.Barrand : remove (very) old configs.*/

#define KUIP_VERSION 950303

#ifdef linux
#  define MACHINE_NAME "LINUX"
#  define OS_NAME "UNIX"
#  define UNIX
#  define HAVE_STRCASECMP
#  define SLASHES "/"
#ifndef HAVE_MEMMOVE
#  define HAVE_MEMMOVE
#endif
#ifndef HAVE_SELECT
#  define HAVE_SELECT
#endif
#endif


#ifdef __APPLE__
#  define MACHINE_NAME "MACOSX"
#  define OS_NAME "UNIX"
#  define UNIX
#  define HAVE_STRCASECMP
#  define SLASHES "/"
#ifndef HAVE_MEMMOVE
#  define HAVE_MEMMOVE
#endif
#ifndef HAVE_SELECT
#  define HAVE_SELECT
#endif
#endif

#ifdef _WIN32
#  define WINNT
#  define MACHINE_NAME "IBMPC"
#  define OS_NAME "WINNT"
#  define UNIX
#  define MSDOS
#  include <io.h>
#  include <process.h>
#  include <direct.h>
#  include <windows.h>
#  define text_mode__()
#  define NO_EDIT_SERVER
#  define NO_SOCKET_H
#  define NO_SYS_TIME_H
#  define NO_UNISTD_H
#  define SIGNAL_V7
#  define SLASHES "\\"
#endif

/*
#if defined(__CYGWIN__) && defined(__GNUC__)
#  define MACHINE_NAME "CYGWIN"
#  define OS_NAME "UNIX"
#  define UNIX
#  define HAVE_MEMMOVE
#  define HAVE_STRCASECMP
#  define HAVE_SELECT
#  define SLASHES "/"
#endif
*/

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#ifndef  NO_FCNTL_H
#include <fcntl.h>
#endif
#include <math.h>          /* contains strtod() and strtol() on some systems */
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#ifdef DBMALLOC
#include "dbmalloc.h"           /* debug malloc */
extern char* getenv(const char*);
extern double strtod(const char*, char**);
#else
#include <stdlib.h>
#endif
#include <time.h>
#ifndef  NO_UNISTD_H
#include <unistd.h>
#endif


#ifndef HAVE_VFORK
#  define vfork fork
#endif


#include <sys/types.h>
#include <sys/stat.h>
#ifndef NO_SYS_TIME_H
#  include <sys/time.h>       /* struct timeval */
#endif
#ifndef MSDOS
#  include <sys/wait.h>
#  ifdef TERMIO_BSD
#    define TERMIO_MAP_NL     /* need to map NL to NL-CR on output */
#    define HAVE_SELECT
#  endif
#  if !defined(TERMIO_BSD) && !defined(TERMIO_SYSV)
#    define TERMIO_POSIX
#  endif
#  if defined(HAVE_SELECT) && !defined(SELECT_CAST)
#    define SELECT_CAST(fds) fds
#  endif
#  include <pwd.h>
#  ifndef NO_DIRENT_H         /* POSIX opendir() */
#    include <dirent.h>
#  else                       /* BSD opendir() */
#    include <sys/dir.h>      /* plus <sys/types.h> */
#    define dirent   direct   /* struct dirent... */
#    ifndef S_IRUSR
#      define S_IRUSR (S_IREAD)   /* read permission, owner */
#      define S_IWUSR (S_IWRITE)  /* write permission, owner */
#      define S_IXUSR (S_IEXEC)   /* execute/search permission, owner */
#    endif
#  endif
#endif
#define HAVE_STAT_H
#ifndef NO_EDIT_SERVER
#  define USE_EDIT_SERVER
#  ifndef F_LOCK              /* BSD file locking */
#    include <sys/file.h>
#    define lockf(fd,op,offs) flock(fd,op)
#    define F_LOCK  LOCK_EX
#    define F_ULOCK LOCK_UN
#  endif
#endif

#ifdef HAVE_STAT_H
#  define KmTimeStamp               struct stat
#  define get_stamp(path,stamp)     stat(path,stamp)
#  define same_stamp(stamp1,stamp2) ((stamp2)->st_mtime == (stamp1)->st_mtime)
#endif

#ifndef KmTimeStamp
#  define KmTimeStamp               int
#  define get_stamp(path,stamp)     (*(stamp) = kuip_checksum(path))
#  define same_stamp(stamp1,stamp2) (*(stamp1) == *(stamp2))
#endif


/* command line arguments recognized by KUARGS */
#if !defined(ARG_STYLE_CMS) && !defined(ARG_STYLE_VMS)
#  define ARG_STYLE_UNIX
#endif


/* #define EXTERN must be in one routine to allocate space for globals */
#ifndef EXTERN
#  define EXTERN extern
#endif


/*
 * Preprocessor syntax for token concatenation
 */
#ifndef ConCat
#  define ConCat(con,cat) con##cat
#endif


/*
 * Prototyping for C functions
 */
#define C_PROTO_0(name) \
                  name(void)
#define C_PROTO_1(name,arg1) \
                  name(arg1)
#define C_PROTO_2(name,arg1,arg2) \
                  name(arg1,arg2)
#define C_PROTO_3(name,arg1,arg2,arg3) \
                  name(arg1,arg2,arg3)
#define C_PROTO_4(name,arg1,arg2,arg3,arg4) \
                  name(arg1,arg2,arg3,arg4)
#define C_PROTO_5(name,arg1,arg2,arg3,arg4,arg5) \
                  name(arg1,arg2,arg3,arg4,arg5)
#define C_PROTO_6(name,arg1,arg2,arg3,arg4,arg5,arg6) \
                  name(arg1,arg2,arg3,arg4,arg5,arg6)
#define C_PROTO_7(name,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
                  name(arg1,arg2,arg3,arg4,arg5,arg6,arg7)
#define C_PROTO_8(name,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8) \
                  name(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
#define C_PROTO_9(name,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9) \
                  name(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)
#define C_PROTO_13(name,a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3) \
                   name(a1,a2,a3,a4,a5,a6,a7,a8,a9,b0,b1,b2,b3)

#define C_DECL_1(name,t1,p1) \
                 name(t1 p1)
#define C_DECL_2(name,t1,p1,t2,p2) \
                 name(t1 p1,t2 p2)
#define C_DECL_3(name,t1,p1,t2,p2,t3,p3) \
                 name(t1 p1,t2 p2,t3 p3)
#define C_DECL_4(name,t1,p1,t2,p2,t3,p3,t4,p4) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4)
#define C_DECL_5(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5)
#define C_DECL_6(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5,t6,p6) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5,t6 p6)
#define C_DECL_7(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5,t6,p6,t7,p7) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5,t6 p6,t7 p7)
#define C_DECL_8(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5,t6,p6,t7,p7,t8,p8) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5,t6 p6,t7 p7,t8 p8)
#define C_DECL_9(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5,t6,p6,t7,p7,t8,p8,t9,p9)\
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5,t6 p6,t7 p7,t8 p8,t9 p9)
#define C_DECL_13(name,t1,p1,t2,p2,t3,p3,t4,p4,t5,p5,t6,p6,t7,p7,t8,p8,\
t9,p9,t10,p10,t11,p11,t12,p12,t13,p13) \
                 name(t1 p1,t2 p2,t3 p3,t4 p4,t5 p5,t6 p6,t7 p7,t8 p8,\
t9 p9,t10 p10,t11 p11,t12 p12,t13 p13)


#ifdef __cplusplus
extern "C" { /* G.Barrand */
#endif

typedef  int      IntFunc();
typedef  char*    CharFunc();
typedef  char**  pCharFunc();
typedef  void     VoidFunc(); /*G.Barrand.*/
  
#ifdef __cplusplus
}
#endif


#endif //kuip_ksys_h
