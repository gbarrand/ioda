// Classes Angle Vector3d
//      B. Revenu , G. Le Meur  2000
//      R. Ansari 2006
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#include "sopnamsp.h"
#include "machdefs.h"
#include <math.h>
#include "pexceptions.h"
#include "vector3d.h"
#include "utilgeom.h"

// Class de conversion d'angles R. Ansari , Juin 2006 
double Angle::_deg2rad = M_PI/180.;
double Angle::_rad2deg = 180./M_PI;
double Angle::_rad2min = 180./M_PI*60.;
double Angle::_rad2sec = 180./M_PI*3600.;

/*!
   \class SOPHYA::Angle
   \ingroup SkyMap
   \brief  Class to ease angle conversions (radian <> degree <> arcmin <> arcsec).
   The angle value is kept in radians internally.
   \code
   // Example to convert 0.035 radians to arcsec
   double vr = 0.035;
   cout << "Angle rad= " << vr << " ->arcsec= " << Angle(vr).ToArcSec() << endl;
   // Example to convert 2.3 arcmin to radian - we use the conversion operator
   double vam = 2.3;
   cout << "Angle arcmin= " << vam << " ->rad= " 
        << (double)Angle(vam, Angle::ArcMin) << endl;
   \endcode

*/

Angle::Angle(double val, Angle::AngleUnit un)
{
  switch (un) {
  case Angle::Radian :
    _angrad = val;
    break;
  case Angle::Degree :
    _angrad = val*_deg2rad;
    break;
  case Angle::ArcMin :
    _angrad = val/_rad2min;
    break;
  case Angle::ArcSec :
    _angrad = val/_rad2sec;
    break;
  default:
    _angrad = val;
    break;
  }
}

//   3-D Geometry 
//        B. Revenu, G. Le Meur   2000
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

/*!
   \class SOPHYA::Vector3d
   \ingroup SkyMap
   \brief 3-D geometry. 
   All computations are made with angles in radians and with spherical
   coordinates theta, phi.
   Concerning Euler's angles, the reference is :

   "Classical Mechanics" 2nd edition, H. Goldstein, Addison Wesley
*/

//! default constructor - unit vector along x direction
Vector3d::Vector3d()
{
  Setxyz(1.,0.,0.);
}

//! Constructor with specification of cartesian coordinates
Vector3d::Vector3d(double x, double y, double z) 
{
  _x=x;
  _y=y;
  _z=z;
  xyz2ThetaPhi();
}
//! Constructor: unit vector with direction (spherical coordinates) specification 
Vector3d::Vector3d(double theta, double phi) 
{
 // _theta=mod(theta,M_PI); // dans [0;pi]
 // Version precedente fausse: _theta=M_PI est valide. Or mod(M_PI,M_PI)=0!
 // De plus theta>pi ou <0 n'a pas de sens. Dominique Yvon
	if( (theta<0.) || (theta>M_PI) )
	{	string exmsg = "Wrong initialisation of theta in Vector3d::Vector3d(double theta, double phi)";
		throw( ParmError(exmsg) );
	}
  _theta=theta; // dans [0;pi]
  _phi=mod(phi,pi2); // dans [0;2pi]
  ThetaPhi2xyz();
}

//! Constructor: unit vector with longitude-latitude specification 
Vector3d::Vector3d(const LongLat& ll)
{
  _theta=ll.Theta(); // dans [0;pi]
  _phi=ll.Phi(); // dans [0;2pi]
  ThetaPhi2xyz();
}

//! Copy constructor
Vector3d::Vector3d(const Vector3d& v) 
{
  _x=v._x;
  _y=v._y;
  _z=v._z;
  _theta=v._theta;
  _phi=v._phi;
}

//! Set/changes the vector direction (result is a unit vector)
void Vector3d::SetThetaPhi(double theta, double phi) 
{
  _theta=mod(theta,M_PI);
  _phi=mod(phi,pi2);
  ThetaPhi2xyz();
}

//! Set/changes the vector specifying cartesian coordinates
void Vector3d::Setxyz(double x, double y, double z) 
{
  _x=x;
  _y=y;
  _z=z;
  xyz2ThetaPhi();
}
//++
void Vector3d::ThetaPhi2xyz()
//
//-- 
{
  _x=sin(_theta)*cos(_phi);
  _y=sin(_theta)*sin(_phi);
  _z=cos(_theta);
}
//++
void Vector3d::xyz2ThetaPhi()
//
//-- 
{
  double norm=this->Norm();
  if( norm != 0. ) 
    {
      _theta=acos(_z/norm); // dans [0,Pi]
      if( mod(_theta,M_PI) == 0. ) _phi=0.; // on est sur +-Oz, le vecteur z est en phi=0
      //      else _phi=acos(_x/sin(_theta)/norm)+M_PI*(_y<0);
      else _phi=scangle(_y/sin(_theta)/norm,_x/sin(_theta)/norm);
    }
  else // vecteur nul
    {
      _theta=0.;
      _phi=0.;
    }
}
//! Normalize the vector (-> unit length) for non zero vectors
Vector3d& Vector3d::Normalize() 
{
  double norm=this->Norm();
  if( norm != 0. )  (*this)/=norm;
  //DEL  else cerr << "Division par zero" << endl;
  return *this;
} 

//! Return the vector norm (length)
double Vector3d::Norm() const 
{
  return sqrt(_x*_x+_y*_y+_z*_z);
}

//! Return the scalar (dot) product of the two vectors
double Vector3d::Psc(const Vector3d& v) const 
{
  return _x*v._x+_y*v._y+_z*v._z;
}
//++
double Vector3d::SepAngle(const Vector3d& v) const 
//
//    angular gap between 2 vectors in [0,Pi]
//-- 
{
  double n1=this->Norm();
  double n2=v.Norm();
  double ret;
  if( n1!=0. && n2!=0. ) ret=acos((this->Psc(v))/n1/n2);
  else 
    {
      cerr << "Division par zero" << endl;
      ret=0.;
    }
  return ret;
}
//++
Vector3d Vector3d::Vect(const Vector3d& v) const 
//
//    vector product
//-- 
{
  double xo=_y*v._z-_z*v._y;
  double yo=_z*v._x-_x*v._z;
  double zo=_x*v._y-_y*v._x;
  return Vector3d(xo,yo,zo);
}
//++
Vector3d Vector3d::VperpPhi() const 
//
//    perpendicular vector, with equal phi
//-- 
{
  double theta;
  if( _theta != pi_over_2 ) theta=_theta+(0.5-(_theta>pi_over_2))*M_PI; // on tourne theta de +-pi/2
  else theta=0.;
  return Vector3d(theta,_phi);
}
//++
Vector3d Vector3d::VperpTheta() const 
//
//    perpendicular vector with equal theta
//-- 
{ cerr<< " Erreur in Vector3d::VperpTheta()"<<endl;
  throw PError("Vector3d::VperpTheta() - Logic Error DY/Reza 20/02/2000"); 
  // Bug ??? (D. Yvon, Fevrier 2000)
  //  double phi=mod(_phi+pi_over_2,pi2); // on tourne phi de pi/2
  //  return Vector3d(_theta,phi);
}

Vector3d Vector3d::EPhi() const 
{
  Vector3d temp;
  if ( fabs(_z) == 1. ) // si on est en +- Oz
    {
      temp=Vector3d(1.,0.,0.);
    }
  else 
    {
      Vector3d k(0,0,-1);
      temp=this->Vect(k);
      temp.Normalize();
    }
  return temp;
}
//++
Vector3d Vector3d::ETheta() const 
//
//-- 
{
  Vector3d temp=this->Vect(EPhi());
  temp.Normalize();
  return temp;
}

//++
Vector3d Vector3d::Euler(double phi, double theta, double psi) const 
//
//    Euler's rotations
//-- 
{
  double cpsi=cos(psi);
  double ctheta=cos(theta);
  double cphi=cos(phi);
  double spsi=sin(psi);
  double stheta=sin(theta);
  double sphi=sin(phi);
  double xnew=(cpsi*cphi-ctheta*sphi*spsi)*_x
    +(cpsi*sphi+ctheta*cphi*spsi)*_y
    +spsi*stheta*_z;
  double ynew=(-spsi*cphi-ctheta*sphi*cpsi)*_x
    +(-spsi*sphi+ctheta*cphi*cpsi)*_y
    +cpsi*stheta*_z;
  double znew=stheta*sphi*_x-stheta*cphi*_y+ctheta*_z;
  return Vector3d(xnew,ynew,znew);
}
//++
Vector3d Vector3d::InvEuler(double phi, double theta, double psi) const 
//
//    inverse rotation
//-- 
{
  double cpsi=cos(psi);
  double ctheta=cos(theta);
  double cphi=cos(phi);
  double spsi=sin(psi);
  double stheta=sin(theta);
  double sphi=sin(phi);
  double xnew=(cpsi*cphi-ctheta*sphi*spsi)*_x
    -(spsi*cphi+ctheta*sphi*cpsi)*_y
    +sphi*stheta*_z;
  double ynew=(cpsi*sphi+ctheta*cphi*spsi)*_x
    +(-spsi*sphi+ctheta*cphi*cpsi)*_y
    -cphi*stheta*_z;
  double znew=stheta*spsi*_x+stheta*cpsi*_y+ctheta*_z;
  return Vector3d(xnew,ynew,znew);
}
//++
Vector3d Vector3d::Rotate(const Vector3d& omega, double phi) const
//
//    rotation of angle phi around an axis omega (Maxwell's rule)
//-- 
{
  Vector3d rotationaxis=omega;
  rotationaxis.Normalize();
  double n=this->Psc(rotationaxis);
  Vector3d myself=*this;
  Vector3d rotate=n*rotationaxis+(myself-n*rotationaxis)*cos(phi)-(myself^rotationaxis)*sin(phi);
  return rotate;
} 
//++  
void Vector3d::Print(ostream& os) const 
//
//--
{
  os << "Vector3: (X,Y,Z)= (" << _x << ", " << _y << ", " << _z 
     << ") --- Theta,Phi= " << _theta << ", " << _phi << "\n"
     << "Norme = " << this->Norm() << endl;
}
//++
// Titre	Operators
//--
//++
Vector3d& Vector3d::operator += (const Vector3d& v) 
//
//-- 
{
  *this=*this+v;
  return *this;
}
//++
Vector3d& Vector3d::operator -= (const Vector3d& v) 
//
//-- 
{
  *this=*this-v;
  return *this;
}
//++
Vector3d& Vector3d::operator += (double d) 
//
//-- 
{
  Setxyz(_x+d,_y+d,_z+d);
  return *this;
}
//++
Vector3d& Vector3d::operator /= (double d) 
//
//-- 
{
  if( d != 0. ) Setxyz(_x/d,_y/d,_z/d);
  else cerr << "Division par zero." << endl;
  return *this;
}
//++
Vector3d& Vector3d::operator *= (double d) 
//
//-- 
{
  Setxyz(_x*d,_y*d,_z*d);
  return *this;
}
//++
Vector3d Vector3d::operator ^ (const Vector3d& v) const 
//
//    vector product
//-- 
{
  return this->Vect(v);
}
//++
Vector3d Vector3d::operator + (double d) const 
//
//-- 
{
  return Vector3d(_x+d,_y+d,_z+d);
}
//++
Vector3d Vector3d::operator + (const Vector3d& v) const 
//
//-- 
{
  return Vector3d(_x+v._x,_y+v._y,_z+v._z);
}
//++
Vector3d Vector3d::operator - (double d) const 
//
//-- 
{
  return *this+(-d);
}
//++
Vector3d Vector3d::operator - (const Vector3d& v) const 
//
//-- 
{
  return *this+(v*(-1.));
}
//++
Vector3d Vector3d::operator * (double d) const 
//
//-- 
{
  return Vector3d(d*_x,d*_y,d*_z);
}
//++
double Vector3d::operator * (const Vector3d& v) const 
//
//    dot product
//-- 
{
  return this->Psc(v);
}
//++
Vector3d Vector3d::operator / (double d) const 
//
//-- 
{
  Vector3d ret=*this;
  if( d != 0. ) ret/=d;
  else  cerr << "Division par zero." << endl;
  return ret;
}
//++
Vector3d& Vector3d::operator = (const Vector3d& v) 
//
//-- 
{
  if( this != &v ) 
    {
      _x=v._x; 
      _y=v._y; 
      _z=v._z; 
      _theta=v._theta; 
      _phi=v._phi;
    }
  return *this;
}
//++
bool Vector3d::operator == (const Vector3d& v) 
//
//-- 
{
  return (this==&v);
}

