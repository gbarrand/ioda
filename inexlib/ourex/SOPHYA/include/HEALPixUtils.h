#ifndef HEALPixUtils_SEEN
#define HEALPixUtils_SEEN
#include "machdefs.h"
#include "ndatablock.h"

// utilitaires de pixelisation HEALPix


namespace SOPHYA {

  // Utility class to compute index to/from angle conversion in HEALPix 
  // spherical map pixelisation

class HEALPix {
public :
  static int_4  nest2ring(int_4 nside,int_4 ipnest) ;
  static int_4  ring2nest(int_4 nside,int_4 ipring) ;

  static int_4  ang2pix_ring(int_4 nside,double theta,double phi) ;
  static int_4  ang2pix_nest(int_4 nside,double theta,double phi) ;
  static void   pix2ang_ring(int_4 nside,int_4 ipix,double& theta,double& phi) ;
  static void   pix2ang_nest(int_4 nside,int_4 ipix,double& theta,double& phi) ;
  
  static int_4  ResolToSizeIndex(double res);  
  static inline int_4  ResolToNSide(double res) 
  { return   ResolToSizeIndex(res); }
  static double SizeIndexToResol(int_4 m);  
  static inline double  NSideToResol(int_4 m) 
  { return  SizeIndexToResol(m); }
};



} // Fin du namespace

#endif
