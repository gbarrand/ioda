//   3-D Geometry 
//        B. Revenu, G. Le Meur   2000
// DAPNIA/SPP (Saclay) / CEA    LAL - IN2P3/CNRS  (Orsay)

#ifndef UNITVECTOR_H_SEEN
#define UNITVECTOR_H_SEEN

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "vector3d.h"

namespace SOPHYA { 

class UnitVector : public Vector3d 
{
  
 public:
  
  UnitVector();
  UnitVector(double x, double y, double z);
  UnitVector(double theta, double phi=0);
  UnitVector(const Vector3d&);
  
  virtual ~UnitVector() { }

  /*virtual*/ Vector3d& operator=(const Vector3d&);  //$CHECK$ EA 101299 operator = non virtual
  virtual Vector3d& operator+=(const Vector3d&);
  virtual Vector3d& operator-=(const Vector3d&);
  virtual Vector3d operator+(const Vector3d&) const;
  virtual Vector3d operator-(const Vector3d&) const;

  virtual Vector3d& operator+=(double d) { return Vector3d::operator+= (d); }
  virtual Vector3d operator+(double d) const { return Vector3d::operator+ (d); }
  virtual Vector3d operator-(double d) const { return Vector3d::operator- (d); }

  void Print(ostream& os) const;
  
};

} // namespace SOPHYA

#endif


