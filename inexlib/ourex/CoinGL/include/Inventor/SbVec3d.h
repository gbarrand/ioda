#ifndef COIN_SBVEC3D_H
#define COIN_SBVEC3D_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

#include <stdio.h>
#include <Inventor/SbBasic.h>

class SbVec3f;
class SbVec3b;
class SbVec3s;
class SbVec3i32;

class COIN_DLL_API SbVec3d {
public:
  SbVec3d(void) { }
  SbVec3d(const double v[3]);
  SbVec3d(const double x, const double y, const double z);
  SbVec3d(const SbVec3f & v);
  explicit SbVec3d(const SbVec3b & v) { setValue(v); }
  explicit SbVec3d(const SbVec3s & v) { setValue(v); }
  explicit SbVec3d(const SbVec3i32 & v) { setValue(v); }

  SbVec3d & setValue(const double v[3]);
  SbVec3d & setValue(const double x, const double y, const double z);
  SbVec3d & setValue(const SbVec3d & barycentric,
                     const SbVec3d & v0,
                     const SbVec3d & v1,
                     const SbVec3d & v2);
  SbVec3d & setValue(const SbVec3f & v);
  SbVec3d & setValue(const SbVec3b & v);
  SbVec3d & setValue(const SbVec3s & v);
  SbVec3d & setValue(const SbVec3i32 & v);

  SbVec3d cross(const SbVec3d & v) const;
  double dot(const SbVec3d & v) const;
  SbBool equals(const SbVec3d & v, const double tolerance) const;
  SbVec3d getClosestAxis(void) const;
  const double * getValue(void) const;
  void getValue(double & x, double & y, double & z) const;
  double length(void) const;
  double sqrLength() const;
  void negate(void);
  double normalize(void);
  double & operator [](const int i);
  const double & operator [](const int i) const;
  SbVec3d & operator *=(const double d);
  SbVec3d & operator /=(const double d);
  SbVec3d & operator +=(const SbVec3d & u);
  SbVec3d & operator -=(const SbVec3d & u);
  SbVec3d operator -(void) const;
  friend COIN_DLL_API SbVec3d operator *(const SbVec3d & v, const double d);
  friend COIN_DLL_API SbVec3d operator *(const double d, const SbVec3d & v);
  friend COIN_DLL_API SbVec3d operator /(const SbVec3d & v, const double d);
  friend COIN_DLL_API SbVec3d operator +(const SbVec3d & v1, const SbVec3d & v2);
  friend COIN_DLL_API SbVec3d operator -(const SbVec3d & v1, const SbVec3d & v2);
  friend COIN_DLL_API int operator ==(const SbVec3d & v1, const SbVec3d & v2);
  friend COIN_DLL_API int operator !=(const SbVec3d & v1, const SbVec3d & v2);

  void print(FILE * fp) const;

private:
  double vec[3];
};

COIN_DLL_API SbVec3d operator *(const SbVec3d & v, const double d);
COIN_DLL_API SbVec3d operator *(const double d, const SbVec3d & v);
COIN_DLL_API SbVec3d operator /(const SbVec3d & v, const double d);
COIN_DLL_API SbVec3d operator +(const SbVec3d & v1, const SbVec3d & v2);
COIN_DLL_API SbVec3d operator -(const SbVec3d & v1, const SbVec3d & v2);
COIN_DLL_API int operator ==(const SbVec3d & v1, const SbVec3d & v2);
COIN_DLL_API int operator !=(const SbVec3d & v1, const SbVec3d & v2);


/* inlined methods ********************************************************/

inline double &
SbVec3d::operator [](const int i)
{
  return this->vec[i];
}

inline const double &
SbVec3d::operator [](const int i) const
{
  return this->vec[i];
}

#endif // !COIN_SBVEC3D_H
