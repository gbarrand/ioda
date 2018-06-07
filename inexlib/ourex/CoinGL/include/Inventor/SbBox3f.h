#ifndef COIN_SBBOX3F_H
#define COIN_SBBOX3F_H

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

#include <Inventor/SbVec3f.h>

class SbBox3d;
class SbBox3i32;
class SbBox3s;

class SbMatrix;

class COIN_DLL_API SbBox3f {
public:
  SbBox3f(void);
  SbBox3f(const float minx, const float miny, const float minz,
          const float maxx, const float maxy, const float maxz);
  SbBox3f(const SbVec3f & min, const SbVec3f & max);
  explicit SbBox3f(const SbBox3d & box) { setBounds(box); }
  explicit SbBox3f(const SbBox3s & box) { setBounds(box); }
  explicit SbBox3f(const SbBox3i32 & box) { setBounds(box); }
  ~SbBox3f();

  void setBounds(const float minx, const float miny, const float minz,
                 const float maxx, const float maxy, const float maxz);
  void setBounds(const SbVec3f & min, const SbVec3f & max);
  SbBox3f & setBounds(const SbBox3d & box);
  SbBox3f & setBounds(const SbBox3s & box);
  SbBox3f & setBounds(const SbBox3i32 & box);

  void getBounds(float & minx, float & miny, float & minz,
                 float & maxx, float & maxy, float & maxz) const;
  void getBounds(SbVec3f & min, SbVec3f & max) const;

  const SbVec3f & getMin(void) const;
  SbVec3f & getMin(void);
  const SbVec3f & getMax(void) const;
  SbVec3f & getMax(void);

  void extendBy(const SbVec3f & point);
  void extendBy(const SbBox3f & box);
  void transform(const SbMatrix & matrix);
  void makeEmpty(void);
  SbBool isEmpty(void) const;
  SbBool hasVolume(void) const;
  float getVolume(void) const;

  SbBool intersect(const SbVec3f & point) const;
  SbBool intersect(const SbBox3f & box) const;
  SbVec3f getClosestPoint(const SbVec3f & point) const;
  SbBool outside(const SbMatrix & mvp, int & cullbits) const;

  SbVec3f getCenter(void) const;
  void getOrigin(float & x0, float & y0, float & z0) const;
  void getSize(float & dx, float & dy, float & dz) const;

  void getSpan(const SbVec3f & dir, float & dmin, float & dmax) const;

  friend COIN_DLL_API int operator ==(const SbBox3f & b1, const SbBox3f & b2);
  friend COIN_DLL_API int operator !=(const SbBox3f & b1, const SbBox3f & b2);

  void print(FILE * file) const;

private:
  SbVec3f min, max;
};

COIN_DLL_API int operator ==(const SbBox3f & b1, const SbBox3f & b2);
COIN_DLL_API int operator !=(const SbBox3f & b1, const SbBox3f & b2);

#endif // !COIN_SBBOX3F_H
