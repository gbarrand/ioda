/* quelques  constantes mathematiques utiles */
#ifndef SMATHCONST_H_SEEN
#define SMATHCONST_H_SEEN

#include "machdefs.h"
#ifndef NO_VALUES_H
#include <values.h>
#else
#include <math.h>
#endif

/* EA conflit entre les diverses copies de nbmath.h, nbconst.h... A nettoyer... */
#undef  Pi   
#undef  Pis2
#undef  DeuxPi 
#undef  SPi   
#undef  S2Pi   
#undef  Rac2   
#undef  Log2   
#undef  LnPi   
#undef  LgPi   
#undef  Ln10   
#undef  DftoDm 
#undef  Hln2pi 
#undef  JourSec 
#undef  AnSec  

#undef  GRAND  
#undef  GRAND2 
#undef  IGRAND 

#define  Pi     (double) ( 3.14159265358979310e+00)  /* c'est Pi */
#define  Pis2   (double) (1.57079632679489655)       /* c'est Pi/2 */
#define  DeuxPi (double) ( 6.28318530717958620e+00)  /* c'est 2*Pi */
#define  SPi    (double) ( 1.77245385090551590e+00)  /* c'est sqrt(Pi) */
#define  S2Pi   (double) ( 2.50662827463100020e+00)  /* c'est sqrt(2*Pi) */
#define  Rac2   (double) ( 1.41421356237309510e+00)  /* c'est sqrt(2) */
#define  Log2   (double) ( 3.01029995663981200e-01)  /* c'est log10(2) */
#define  LnPi   (double) ( 1.14472988584940020e+00)  /* c'est ln(Pi) */
#define  LgPi   (double) ( 4.97149872694133850e-01)  /* c'est log10(Pi) */
#define  Ln10   (double) ( 2.30258509299404590e+00)  /* c'est ln(10) */
#define  DftoDm (double) ( 1.08573620475812940e+00)  /* c'est 2.5/ln(10) */
#define  Hln2pi (double) ( 9.18938533204672670e-01)  /* c'est Ln(2*pi)/2 */

#define  JourSec (float) (86400.000000) /* nombre de secondes dans 24H */
#define  AnSec  (int) (31557600) /* nombre de secondes dans 365.25 jours */

#define  GRAND  (float)  (1.e+35)
#define  GRAND2 (double) (1.e+35)
#define  IGRAND (int_4) (2147483647)


#endif
