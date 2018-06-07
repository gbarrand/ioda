/* G.Barrand : have the strict necessary to pass "smart Sophya". */

#ifndef MACHDEFS_SEEN
#define MACHDEFS_SEEN

#ifdef SOPHYA_DEBUG
#define SO_BOUNDCHECKING
#endif

#if defined(OSF1) 
#define IS_BIG_ENDIAN 0
#endif

#ifdef __APPLE__
#ifdef __MACH__
#ifdef __POWERPC__
#else
#define IS_BIG_ENDIAN 0
#endif
#endif
#endif

#if defined(Linux)  || defined(linux)
#if defined(__alpha__)
#define IS_BIG_ENDIAN 0
#elif defined(powerpc)
#elif defined(__i386__)    
#define IS_BIG_ENDIAN 0
#else
#define IS_BIG_ENDIAN 0
#endif
#endif

typedef signed char int_1;
typedef unsigned char uint_1;
typedef short int_2;
typedef unsigned short uint_2;
typedef int int_4;
typedef unsigned int uint_4;
typedef float r_4;
typedef double r_8;

#if defined(_MSC_VER)
typedef __int64 int_8;
typedef unsigned __int64 uint_8;
#elif defined(_LP64) 
typedef long int_8;
typedef unsigned long uint_8;
#else /* 32-Bit Platforms */
typedef long long int_8;
typedef unsigned long long uint_8;
#endif

#ifndef IS_BIG_ENDIAN
#define IS_BIG_ENDIAN 1
#endif

/* ---- La taille des index des tableaux ---- */
#ifdef SO_SASIZET64 
typedef int_8 sa_size_t ;
#else
typedef int_4 sa_size_t ;
#endif

#define DECL_TEMP_SPEC template <>
#define TSNMLUPG4
#define ANSI_TEMPLATES

#ifdef __APPLE__
#define NO_VALUES_H
#define BITSPERBYTE     8
#define BITS(type)      (BITSPERBYTE * (long)sizeof (type))
#define MAXDOUBLE       1.79769313486231570e+308
#define MINDOUBLE       4.94065645841246544e-324
#define _IEEE           1
#define _DEXPLEN        11
#define _HIDDENBIT      1
#define DMINEXP (-(DMAXEXP + DSIGNIF - _HIDDENBIT - 3))
#define DSIGNIF (BITS(double) - _DEXPLEN + _HIDDENBIT - 1)
//G.Barrand : add () at right of <<
#define DMAXEXP ((1 << (_DEXPLEN - 1)) - 1 + _IEEE)
#endif

#if ANDROID
#define NO_VALUES_H
#define BITSPERBYTE     8
#define BITS(type)      (BITSPERBYTE * (long)sizeof (type))
#define MAXDOUBLE       1.79769313486231570e+308
#define MINDOUBLE       4.94065645841246544e-324
#define _IEEE           1
#define _DEXPLEN        11
#define _HIDDENBIT      1
#define DMINEXP (-(DMAXEXP + DSIGNIF - _HIDDENBIT - 3))
#define DSIGNIF (BITS(double) - _DEXPLEN + _HIDDENBIT - 1)
//G.Barrand : add () at right of <<
#define DMAXEXP ((1 << (_DEXPLEN - 1)) - 1 + _IEEE)
/*
#include <inlib/stl/typeinfo>
#define typeid INLIB_TYPEID
#define dynamic_cast  _dynamic_cast 
double drand48();
*/
#endif

#ifdef _MSC_VER
/* there is no M_PI, M_E in math.h. */
#define M_PI 3.14159265358979323846
#define M_E  2.71828182845904523536028747135266250

#define SO_NOTHSAFE

#define NO_VALUES_H
#define BITSPERBYTE     8
#define BITS(type)      (BITSPERBYTE * (long)sizeof (type))
#define MAXDOUBLE       1.79769313486231570e+308
#define MINDOUBLE       4.94065645841246544e-324
#define _IEEE           1
#define _DEXPLEN        11
#define _HIDDENBIT      1
#define DMINEXP (-(DMAXEXP + DSIGNIF - _HIDDENBIT - 3))
#define DSIGNIF (BITS(double) - _DEXPLEN + _HIDDENBIT - 1)
//G.Barrand : add () at right of <<
#define DMAXEXP ((1 << (_DEXPLEN - 1)) - 1 + _IEEE)

/* missing in math.h : */
#if _MSC_VER <= 1500
#define copysign(a__x,a__what) ((a__what)>=0?(a__x):-(a__x))
#define trunc(a__double) ((a__double)>0?floor(a__double):ceil(a__double))
#define atoll atol
#endif
#define drand48(a__dummy) ((double)rand())/((double)RAND_MAX)
#ifdef __cplusplus
extern "C" {
#endif
  void srand48(long);
  /*double drand48();*/ /*also defined in graphviz!*/
  unsigned short* seed48(unsigned short a_s[3]);
  char* strptime(const char* rbuf,const char* format,struct tm* tm);
#ifdef __cplusplus
}
#endif

/* missing in signal.h : */
#ifdef __cplusplus
typedef unsigned int sigset_t;
struct  sigaction {
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flags;
};
inline int sigaction(int, const struct sigaction*,struct sigaction*) {return -1;}
#endif

#define SIGQUIT  3
#define SIGALRM 14
#define SIGUSR1 30
#define SIGUSR2 31

/* for SUtils/utils.cc : */
#define NPOS std::string::npos

/* for Samba/harmspher.cc : */
#define M_2_SQRTPI  1.12837916709551257389615890312154517   /* 2/sqrt(pi)     */

#endif

#ifdef __cplusplus
namespace std { }
using namespace std;
#endif

#endif
