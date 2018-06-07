#ifndef COIN_SBVEC3S_H
#define COIN_SBVEC3S_H

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

class SbVec3us;
class SbVec3b;
class SbVec3i32;
class SbVec3f;
class SbVec3d;

class COIN_DLL_API SbVec3s {
public:
  SbVec3s(void);
  SbVec3s(const short v[3]);
  SbVec3s(const short x, const short y, const short z);
  explicit SbVec3s(const SbVec3us & v) { setValue(v); }
  explicit SbVec3s(const SbVec3b & v) { setValue(v); }
  explicit SbVec3s(const SbVec3i32 & v) { setValue(v); }
  explicit SbVec3s(const SbVec3f & v) { setValue(v); }
  explicit SbVec3s(const SbVec3d & v) { setValue(v); }

  int32_t dot(const SbVec3s & v) const;
  const short * getValue(void) const;
  void getValue(short & x, short & y, short & z) const;
  void negate(void);
  SbVec3s & setValue(const short v[3]);
  SbVec3s & setValue(const short x, const short y, const short z);
  SbVec3s & setValue(const SbVec3us & v);
  SbVec3s & setValue(const SbVec3b & v);
  SbVec3s & setValue(const SbVec3i32 & v);
  SbVec3s & setValue(const SbVec3f & v);
  SbVec3s & setValue(const SbVec3d & v);

  short & operator [](const int i);
  const short & operator [](const int i) const;
  SbVec3s & operator *=(int d);
  SbVec3s & operator *=(double  d);
  SbVec3s & operator /=(int d);
  SbVec3s & operator /=(double d);
  SbVec3s & operator +=(const SbVec3s & u);
  SbVec3s & operator -=(const SbVec3s & u);
  SbVec3s operator-(void) const;
  friend COIN_DLL_API SbVec3s operator *(const SbVec3s & v, int d);
  friend COIN_DLL_API SbVec3s operator *(const SbVec3s & v, double d);
  friend COIN_DLL_API SbVec3s operator *(int d, const SbVec3s & v);
  friend COIN_DLL_API SbVec3s operator *(double d, const SbVec3s & v);
  friend COIN_DLL_API SbVec3s operator /(const SbVec3s & v, int d);
  friend COIN_DLL_API SbVec3s operator /(const SbVec3s & v, double d);
  friend COIN_DLL_API SbVec3s operator +(const SbVec3s & v1, const SbVec3s & v2);
  friend COIN_DLL_API SbVec3s operator -(const SbVec3s & v1, const SbVec3s & v2);
  friend COIN_DLL_API int operator ==(const SbVec3s & v1, const SbVec3s & v2);
  friend COIN_DLL_API int operator !=(const SbVec3s & v1, const SbVec3s & v2);

  void print(FILE * fp) const;

private:
  short vec[3];
};

COIN_DLL_API SbVec3s operator *(const SbVec3s & v, int d);
COIN_DLL_API SbVec3s operator *(const SbVec3s & v, double d);
COIN_DLL_API SbVec3s operator *(int d, const SbVec3s & v);
COIN_DLL_API SbVec3s operator *(double d, const SbVec3s & v);
COIN_DLL_API SbVec3s operator /(const SbVec3s & v, int d);
COIN_DLL_API SbVec3s operator /(const SbVec3s & v, double d);
COIN_DLL_API SbVec3s operator +(const SbVec3s & v1, const SbVec3s & v2);
COIN_DLL_API SbVec3s operator -(const SbVec3s & v1, const SbVec3s & v2);
COIN_DLL_API int operator ==(const SbVec3s & v1, const SbVec3s & v2);
COIN_DLL_API int operator !=(const SbVec3s & v1, const SbVec3s & v2);

/* inlined methods ********************************************************/

inline short &
SbVec3s::operator [](const int i)
{
  return this->vec[i];
}

inline const short &
SbVec3s::operator [](const int i) const
{
  return this->vec[i];
}

#endif // !COIN_SBVEC3S_H
