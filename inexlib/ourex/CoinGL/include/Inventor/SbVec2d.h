#ifndef COIN_SBVEC2D_H
#define COIN_SBVEC2D_H

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

class SbVec2f;
class SbVec2b;
class SbVec2s;
class SbVec2i32;

class COIN_DLL_API SbVec2d {
public:
  SbVec2d(void);
  SbVec2d(const double v[2]);
  SbVec2d(const double x, const double y);
  explicit SbVec2d(const SbVec2f & v) { setValue(v); }
  explicit SbVec2d(const SbVec2b & v) { setValue(v); }
  explicit SbVec2d(const SbVec2s & v) { setValue(v); }
  explicit SbVec2d(const SbVec2i32 & v) { setValue(v); }

  SbVec2d & setValue(const double v[2]);
  SbVec2d & setValue(const double x, const double y);
  SbVec2d & setValue(const SbVec2f & v);
  SbVec2d & setValue(const SbVec2b & v);
  SbVec2d & setValue(const SbVec2s & v);
  SbVec2d & setValue(const SbVec2i32 & v);

  double & operator [] (const int i);
  const double & operator [] (const int i) const;

  double dot(const SbVec2d & v) const;
  SbBool equals(const SbVec2d & v, const double tolerance) const;
  const double * getValue(void) const;
  void getValue(double & x, double & y) const;
  double length(void) const;
  double sqrLength(void) const;
  void negate(void);
  double normalize(void);
  SbVec2d & operator *= (const double d);
  SbVec2d & operator /= (const double d);
  SbVec2d & operator += (const SbVec2d & u);
  SbVec2d & operator -= (const SbVec2d & u);
  SbVec2d operator -(void) const;

  friend COIN_DLL_API SbVec2d operator * (const SbVec2d & v, const double d);
  friend COIN_DLL_API SbVec2d operator * (const double d, const SbVec2d & v);
  friend COIN_DLL_API SbVec2d operator / (const SbVec2d & v, const double d);
  friend COIN_DLL_API SbVec2d operator + (const SbVec2d & v1, const SbVec2d & v2);
  friend COIN_DLL_API SbVec2d operator - (const SbVec2d & v1, const SbVec2d & v2);
  friend COIN_DLL_API int operator == (const SbVec2d & v1, const SbVec2d & v2);
  friend COIN_DLL_API int operator != (const SbVec2d & v1, const SbVec2d & v2);

  void print(FILE * fp) const;

private:
  double vec[2];
};

COIN_DLL_API SbVec2d operator * (const SbVec2d & v, const double d);
COIN_DLL_API SbVec2d operator * (const double d, const SbVec2d & v);
COIN_DLL_API SbVec2d operator / (const SbVec2d & v, const double d);
COIN_DLL_API SbVec2d operator + (const SbVec2d & v1, const SbVec2d & v2);
COIN_DLL_API SbVec2d operator - (const SbVec2d & v1, const SbVec2d & v2);
COIN_DLL_API int operator == (const SbVec2d & v1, const SbVec2d & v2);
COIN_DLL_API int operator != (const SbVec2d & v1, const SbVec2d & v2);


// *************************************************************************

//$ IMPORT INLINE ../../src/SbVec2d.cpp

// *************************************************************************

#endif // !COIN_SBVEC2D_H
