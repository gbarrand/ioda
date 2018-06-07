/*    Declaration de fonctions math pour float   */

#ifndef  FMATH_H_SEEN
#define  FMATH_H_SEEN

/* float math functions declarations using #define removed
   to avoid compilation problems - Sophya code modified 
   accordingly - 
   C++ overloaded math functions should be used instead
                                        R. Ansari

if defined(OSF1)
define   fexp(x)       expf(x)
define   fsqrt(x)      sqrtf(x)
define   flog10(x)     log10f(x)
define   flog(x)       logf(x)
elif !defined(IRIX64)
define   fexp(x)     ((float)(exp((double)(x))))
define   expf(x)     ((float)(exp((double)(x))))
define   fabsf(x)    ((float)(fabs((double)(x))))
define   fsqrt(x)    ((float)(sqrt((double)(x))))
define   sqrtf(x)    ((float)(sqrt((double)(x))))
define   flog10(x)   ((float)(log10((double)(x))))
define   log10f(x)   ((float)(log10((double)(x))))
define   flog(x)     ((float)(log((double)(x))))
define   logf(x)     ((float)(log((double)(x))))
define   floorf(x)   ((float) floor((double)(x)))
define   ceilf(x)    ((float) ceil((double)(x)))
define   powf(x,y)    ((float) pow((double)(x), (double)(y)))
endif

*/


#endif

