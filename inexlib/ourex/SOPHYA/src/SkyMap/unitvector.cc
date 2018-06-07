//   3-D Geometry 
//        B. Revenu, G. Le Meur   2000
//        R. Ansari (documentation 2006)
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

#include <math.h>
#include "sopnamsp.h"
#include "unitvector.h"

/*!
   \class SOPHYA::UnitVector
   \ingroup SkyMap
   \brief Specialisation of Vector3d class for representing unit (length=1) 3-vectors. 
*/

//! Default constructor : unit vector in the x direction
UnitVector::UnitVector() : Vector3d(1.,0.,0.) 
{
}

//! Constructor: Unit vector along the x,y,z cartesian coordinates
UnitVector::UnitVector(double x, double y, double z) : Vector3d(x,y,z) 
{
  this->Normalize();
}

//! Constructor: Unit vector along the theta,phi direction (spherical coordinates)
UnitVector::UnitVector(double theta, double phi) : Vector3d(theta,phi) 
{
  this->Normalize();
}
//! copy constructor
UnitVector::UnitVector(const Vector3d& v) : Vector3d(v) 
{
  this->Normalize();
}
//! print the vector on stream os
void UnitVector::Print(ostream& os) const 
{
  os << "UnitVector : (X,Y,Z)= (" << _x << ", " << _y << ", " << _z 
     << ") Theta,Phi= " << _theta << ", " << _phi << "\n"
     << "norme =" << this->Norm() << endl;
}
//++
// Titre	Operators
//--
//++
Vector3d& UnitVector::operator = (const Vector3d& v)
//
//--

{
  Setxyz(v.X(),v.Y(),v.Z());
  this->Normalize();
  return *this;
}
//++
Vector3d& UnitVector::operator += (const Vector3d& v)
//
//--
{
  Setxyz(_x+v.X(),_y+v.Y(),_z+v.Z());
  this->Normalize();
  return *this;
}
//++
Vector3d& UnitVector::operator -= (const Vector3d& v)
//
//--
{
  Setxyz(_x-v.X(),_y-v.Y(),_z-v.Z());
  this->Normalize();
  return *this;
}
//++
Vector3d UnitVector::operator + (const Vector3d& v) const
//
//--
{
  return UnitVector(_x+v.X(),_y+v.Y(),_z+v.Z());
}
//++
Vector3d UnitVector::operator - (const Vector3d& v) const
//
//--
{
  return UnitVector(_x-v.X(),_y-v.Y(),_z-v.Z());
}
