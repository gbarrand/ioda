//   Angles/geometrie 3D
//        B. Revenu 2000
//        R. Ansari 2006
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

#ifndef LONGLAT_H_SEEN
#define LONGLAT_H_SEEN

#include "machdefs.h"
#include <math.h>
#include <iostream>
#include <stdio.h>


#include <string.h>
#include "utilgeom.h"

/*
  passage des coordonnees longitude, latitude en theta,phi
  longitude=phi
  latitude=Pi/2-theta
*/

namespace SOPHYA {  

class LongLat
{
  
 public:
  
  LongLat();
  LongLat(double longitude, double latitude);
  LongLat(double x, double y, double z);
  LongLat(const LongLat&);

  //! Set the longitude/latitude coordinates (in radians)  
  void Set(double longitude, double latitude);
  //! Return the longitude value (in radians) 
  double Longitude() const {return _lon;}
  //! Return the phi value (in radians) 
  double Phi() const {return _lon;}
  //! Return the latitude value (in radians) 
  double Latitude() const {return _lat;}
  //! Return the theta value (in radians) 
  double Theta() const {return pi_over_2-_lat;}

  //! print the object (ascii representation) on stream os 
  void Print(ostream& os) const;
  
 private:
  
  double _lon; // lambda
  double _lat; // beta
  
};

inline ostream& operator<<(ostream& s, const LongLat& ll) 
{  
  ll.Print(s);  
  return s;  
}

} // namespace SOPHYA

#endif
