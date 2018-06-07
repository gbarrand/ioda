#ifndef COIN_SBBOX2F_H
#define COIN_SBBOX2F_H

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

#include <Inventor/SbVec2f.h>

class SbBox2d;
class SbBox2s;
class SbBox2i32;

class COIN_DLL_API SbBox2f {
public:
  SbBox2f(void);
  SbBox2f(float xmin, float ymin, float xmax, float ymax);
  SbBox2f(const SbVec2f & min, const SbVec2f & max);
  explicit SbBox2f(const SbBox2d & box) { setBounds(box); }
  explicit SbBox2f(const SbBox2s & box) { setBounds(box); }
  explicit SbBox2f(const SbBox2i32 & box) { setBounds(box); }
  ~SbBox2f(void);

  void setBounds(float xmin, float ymin, float xmax, float ymax);
  void setBounds(const SbVec2f & min, const SbVec2f & max);
  SbBox2f & setBounds(const SbBox2d & box);
  SbBox2f & setBounds(const SbBox2s & box);
  SbBox2f & setBounds(const SbBox2i32 & box);

  void getBounds(float & xmin, float & ymin, float & xmax, float & ymax) const;
  void getBounds(SbVec2f & min, SbVec2f & max) const;

  const SbVec2f & getMin(void) const;
  SbVec2f & getMin(void) { return minpt; }
  const SbVec2f & getMax(void) const;
  SbVec2f & getMax(void) { return maxpt; }

  void extendBy(const SbVec2f & point);
  void extendBy(const SbBox2f & box);
  void makeEmpty(void);
  SbBool isEmpty(void) const;
  SbBool hasArea(void) const;

  SbBool intersect(const SbVec2f & point) const;
  SbBool intersect(const SbBox2f & box) const;
  SbVec2f getClosestPoint(const SbVec2f & p) const;

  SbVec2f getCenter(void) const;
  void getOrigin(float & x0, float & y0) const;
  void getSize(float & w, float & h) const;
  float getAspectRatio(void) const;

  friend COIN_DLL_API int operator ==(const SbBox2f & b1, const SbBox2f & b2);
  friend COIN_DLL_API int operator !=(const SbBox2f & b1, const SbBox2f & b2);

private:
  SbVec2f minpt, maxpt;

  float width(void) const;
  float height(void) const;
};

COIN_DLL_API int operator ==(const SbBox2f & b1, const SbBox2f & b2);
COIN_DLL_API int operator !=(const SbBox2f & b1, const SbBox2f & b2);

#endif // !COIN_SBBOX2F_H
