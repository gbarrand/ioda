#ifndef COIN_SBBOX3S_H
#define COIN_SBBOX3S_H

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

#include <Inventor/SbVec3s.h>
#include <Inventor/SbVec3f.h>

class SbBox3i32;
class SbBox3f;
class SbBox3d;

class COIN_DLL_API SbBox3s {
public:
  SbBox3s(void);
  SbBox3s(short xmin, short ymin, short zmin, short xmax, short ymax, short zmax);
  SbBox3s(const SbVec3s & minvec, const SbVec3s & maxvec);
  explicit SbBox3s(const SbBox3i32 & box) { setBounds(box); }
  explicit SbBox3s(const SbBox3f & box) { setBounds(box); }
  explicit SbBox3s(const SbBox3d & box) { setBounds(box); }
  ~SbBox3s(void);

  void setBounds(short xmin, short ymin, short zmin, short xmax, short ymax, short zmax);
  void setBounds(const SbVec3s & minvec, const SbVec3s & maxvec);
  SbBox3s & setBounds(const SbBox3i32 & box);
  SbBox3s & setBounds(const SbBox3f & box);
  SbBox3s & setBounds(const SbBox3d & box);

  const SbVec3s & getMin(void) const;
  SbVec3s & getMin(void) { return minpt; }
  const SbVec3s & getMax(void) const;
  SbVec3s & getMax(void) { return maxpt; }

  void extendBy(const SbVec3s & point);
  void extendBy(const SbBox3s & box);
  void makeEmpty(void);
  SbBool isEmpty(void) const { return (maxpt[0] < minpt[0]); }
  SbBool hasVolume(void) const
    { return ((maxpt[0] > minpt[0]) && (maxpt[1] > minpt[1]) && (maxpt[2] > minpt[2])); }
  int getVolume(void) const
    { short dx = 0, dy = 0, dz = 0; getSize(dx, dy, dz); return (dx * dy * dz); }

  SbBool intersect(const SbVec3s & point) const;
  SbBool intersect(const SbBox3s & box) const;
  void getBounds(short & xmin, short & ymin, 
                 short & zmin, short & xmax, 
                 short & ymax, short & zmax) const;
  void getBounds(SbVec3s & minvec, SbVec3s & maxvec) const;

  SbVec3f getCenter(void) const
    { return SbVec3f((minpt[0]+maxpt[0])*0.5f, (minpt[1]+maxpt[1])*0.5f, (minpt[2]+maxpt[2])*0.5f); }
  void getOrigin(short & originX, short & originY, short & originZ) const;
  void getSize(short & sizeX, short & sizeY, short & sizeZ) const;

  friend COIN_DLL_API int operator ==(const SbBox3s & b1, const SbBox3s & b2);
  friend COIN_DLL_API int operator !=(const SbBox3s & b1, const SbBox3s & b2);

private:
  SbVec3s minpt, maxpt;

  short width(void) const;
  short height(void) const;
  short depth(void) const;
  SbBool hasArea(void) const;
};

COIN_DLL_API int operator ==(const SbBox3s & b1, const SbBox3s & b2);
COIN_DLL_API int operator !=(const SbBox3s & b1, const SbBox3s & b2);

#endif // !COIN_SBBOX3S_H
