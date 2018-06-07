#ifndef COIN_SBBOX2D_H
#define COIN_SBBOX2D_H

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

#include <Inventor/SbVec2d.h>

class SbBox2f;
class SbBox2s;
class SbBox2i32;

class COIN_DLL_API SbBox2d {
public:
  SbBox2d(void);
  SbBox2d(double xmin, double ymin, double xmax, double ymax);
  SbBox2d(const SbVec2d & min, const SbVec2d & max);
  explicit SbBox2d(const SbBox2f & box) { setBounds(box); }
  explicit SbBox2d(const SbBox2s & box) { setBounds(box); }
  explicit SbBox2d(const SbBox2i32 & box) { setBounds(box); }
  ~SbBox2d(void);

  void setBounds(double xmin, double ymin, double xmax, double ymax);
  void setBounds(const SbVec2d & min, const SbVec2d & max);
  SbBox2d & setBounds(const SbBox2f & box);
  SbBox2d & setBounds(const SbBox2s & box);
  SbBox2d & setBounds(const SbBox2i32 & box);

  void getBounds(double & xmin, double & ymin, double & xmax, double & ymax) const;
  void getBounds(SbVec2d & min, SbVec2d & max) const;

  const SbVec2d & getMin(void) const;
  SbVec2d & getMin(void) { return maxpt; }
  const SbVec2d & getMax(void) const;
  SbVec2d & getMax(void) { return maxpt; }

  void extendBy(const SbVec2d & point);
  void extendBy(const SbBox2d & box);
  void makeEmpty(void);
  SbBool isEmpty(void) const;
  SbBool hasArea(void) const;

  SbBool intersect(const SbVec2d & point) const;
  SbBool intersect(const SbBox2d & box) const;
  SbVec2d getClosestPoint(const SbVec2d & p) const;
  SbBool findIntersection(const SbVec2d & a, const SbVec2d & b, SbVec2d & ia, SbVec2d & ib) const;

  SbVec2d getCenter(void) const;
  void getOrigin(double & x0, double & y0) const;
  void getSize(double & w, double & h) const;
  double getAspectRatio(void) const;

  friend COIN_DLL_API int operator ==(const SbBox2d & b1, const SbBox2d & b2);
  friend COIN_DLL_API int operator !=(const SbBox2d & b1, const SbBox2d & b2);

private:
  SbVec2d minpt, maxpt;

  double width(void) const;
  double height(void) const;
};

COIN_DLL_API int operator == (const SbBox2d & b1, const SbBox2d & b2);
COIN_DLL_API int operator != (const SbBox2d & b1, const SbBox2d & b2);

#endif // !COIN_SBBOX2D_H
