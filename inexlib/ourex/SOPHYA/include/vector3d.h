//   3-D Geometry 
//        B. Revenu, G. Le Meur   2000
//        R. Ansari 2006
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

#ifndef VECTOR3D_H_SEEN
#define VECTOR3D_H_SEEN

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "longlat.h"


namespace SOPHYA { 

//! Class to ease angle conversions (radian <> degree <> arcmin <> arcsec)
class Angle {
public:
  enum AngleUnit { Radian, Degree, ArcMin, ArcSec };
  //! Constructor with specification of angle value in radian 
  Angle(double val=0.) { _angrad = val; }
  //! Constructor with specification of angle value and unit
  Angle(double val, Angle::AngleUnit un);
  //! Copy constructor
  Angle(Angle const& a) { _angrad = a._angrad; }

  //! Conversion to radian
  inline double ToRadian() const { return _angrad; }
  //! Conversion to degree
  inline double ToDegree() const { return _angrad*_rad2deg; }
  //! Conversion to arcmin
  inline double ToArcMin() const { return _angrad*_rad2min; }
  //! Conversion to arcsec
  inline double ToArcSec() const { return _angrad*_rad2sec; }

  //! return the angle value in radian
  inline operator double () const { return _angrad; }

protected:  
  double _angrad;  // angle in radians 

  static double _deg2rad;  // deg -> radian conversion factor
  static double _rad2deg;  // rad -> degree conversion factor
  static double _rad2min;  // rad -> arcmin conversion factor
  static double _rad2sec;  // rad -> arcmin conversion factor
  
};

//! Angle conversion: Radian to degree 
inline double RadianToDegree(double ar) 
{ return Angle(ar).ToDegree(); }
//! Angle conversion: Degree to radian 
inline double DegreeToRadian(double ad) 
{ return Angle(ad,Angle::Degree).ToRadian(); }
//! Angle conversion: Arcminute to radian  
inline double ArcminToRadian(double aam) 
{ return Angle(aam,Angle::ArcMin).ToRadian(); }

/*
  Geometrie en dimension 3. 
  Tous les calculs sont faits en radians 
  et en coordonnees spheriques theta,phi
  pour les rotations (angles d'Euler) ma source est 
  B. Revenu / G. Le Meur
  "Classical Mechanics" 2nd edition, H. Goldstein, Addison Wesley
*/
 
class Vector3d 
{

 public:
  
  Vector3d();
  Vector3d(double x, double y, double z);
  Vector3d(double theta, double phi);
  Vector3d(const LongLat&);
  Vector3d(const Vector3d&);

  virtual ~Vector3d() { } 

//   To manipulate the vector
  virtual void Setxyz(double x, double y, double z);
  virtual void SetThetaPhi(double theta,  double phi);
  virtual void ThetaPhi2xyz();
  virtual void xyz2ThetaPhi();

// Acces to coordinates
  inline double Theta() const {return _theta;}
  inline double Phi() const {return _phi;}
  inline double X() const {return _x;}
  inline double Y() const {return _y;}
  inline double Z() const {return _z;}

  virtual Vector3d& Normalize();
  virtual double Norm() const;

  // produit scalaire
  virtual double Psc(const Vector3d&) const;

  // ecart angulaire entre 2 vecteurs dans [0,Pi]
  //!   angular gap between 2 vectors in [0,Pi] 
  virtual double SepAngle(const Vector3d&) const;

  // produit vectoriel
  //! return the vector product (*this)^v2
  virtual Vector3d Vect(const Vector3d& v2) const;

  // vecteur perpendiculaire de meme phi
  //! return the perpendicular vector, with equal phi 
  virtual Vector3d VperpPhi() const;

  // vecteur perpendiculaire de meme theta
  //! return the perpendicular vector, with equal theta 
  virtual Vector3d VperpTheta() const;

  virtual Vector3d ETheta() const;
  virtual Vector3d EPhi() const;

  // rotations d'Euler
  //! Perform   Euler's rotations 
  virtual Vector3d Euler(double, double, double) const;

  // rotation inverse
  //! perform   inverse Euler rotation 
  Vector3d InvEuler(double, double, double) const;

  // rotation d'angle phi autour d'un axe omega (regle du tire-bouchon)
  //! perform rotation of angle phi around an axis omega (Maxwell's rule) 
  Vector3d Rotate(const Vector3d& omega,double phi) const;

  /*virtual*/ Vector3d& operator=(const Vector3d&); // $CHECK$ EA 101299
  virtual Vector3d& operator+=(const Vector3d&);
  virtual Vector3d& operator-=(const Vector3d&);
  virtual Vector3d operator+(const Vector3d&) const;
  virtual Vector3d operator-(const Vector3d&) const;

  virtual Vector3d& operator+=(double);
  virtual Vector3d& operator/=(double);
  virtual Vector3d& operator*=(double);

  virtual Vector3d operator+(double) const;
  virtual Vector3d operator-(double) const;
  virtual Vector3d operator*(double) const;
  virtual Vector3d operator/(double) const;

  /*!    vector product */
  virtual Vector3d operator^(const Vector3d&) const; // produit vectoriel
  /*!    dot product */
  virtual double operator*(const Vector3d&) const; // produit scalaire

  bool operator==(const Vector3d&);
  
  virtual void Print(ostream& os) const;

 protected:

  double _x;
  double _y;
  double _z;
  double _theta;
  double _phi;

};

inline ostream& operator<<(ostream& s, const Vector3d& v) 
{  
  v.Print(s);  
  return s;  
}

// fonctions globales

inline Vector3d operator*(double d, const Vector3d& v) 
{
  return v*d;
}

inline Vector3d operator+(double d, const Vector3d& v) 
{
  return v+d;
}


} // namespace SOPHYA

#endif


