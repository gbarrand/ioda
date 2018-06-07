//   Angles/geometrie 3D
//        B. Revenu 2000
//        R. Ansari 2006
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

#include "sopnamsp.h"
#include "longlat.h"


/*!
   \class SOPHYA::LongLat
   \ingroup SkyMap
   \brief Coordinate transform from  (longitude,latitude) to (theta,phi)
   All angles are specified in radians
   
   - longitude=phi
   - latitude=Pi/2-theta  (latitude=0 -> equator)
*/

//! Default constructor: longitude=latitude=0 
LongLat::LongLat()
{
  _lon=0.;
  _lat=0.;
}
//! Constructor
LongLat::LongLat(double longitude, double latitude)
{
  _lon=mod(longitude,pi2);
  if( latitude == pi_over_2 ) _lat=latitude;
  else _lat=mod(latitude+pi_over_2,M_PI)-pi_over_2; // dans [-Pi/2,Pi/2]
}

//! Constructor form cartesian coordiantes
LongLat::LongLat(double x, double y, double z)
{
  double norm=sqrt(x*x+y*y+z*z);
  double theta;
  if( norm != 0. ) 
    {
      theta=acos(z/norm); // dans [0,Pi]
      if( mod(theta,M_PI) == 0. ) _lon=0.; // on est sur +-Oz, le vecteur z est en phi=0
      //      else _lon=acos(x/sin(theta)/norm)+M_PI*(y<0);
      else _lon=scangle(y/sin(theta)/norm,x/sin(theta)/norm);
    }
  else // vecteur nul
    {
      theta=0.;
      _lon=0.;
    }
  _lat=pi_over_2-theta;
}
//++
// Titre	Public Methods
//--
//++
void LongLat::Set(double longitude, double latitude)
//
//--
{
  _lon=mod(longitude,pi2);
  if( latitude == pi_over_2 ) _lat=latitude;
  else _lat=mod(latitude+pi_over_2,M_PI)-pi_over_2; // dans [-Pi/2,Pi/2]
}
//++
void LongLat::Print(ostream& os) const
//
//--
{
  os << "LongLat : longitude = " << _lon << " phi   = " << this->Phi() << endl;
  os << "LongLat : latitude    = " << _lat << " theta = " << this->Theta() << endl;
}
//++
//
//  double Longitude() const 
//  double Phi() const 
//  double Latitude() 
//  double Theta() const 
//--
