#ifndef COIN_SBBOX2S_H
#define COIN_SBBOX2S_H

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

#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec2f.h>

class SbBox2i32;
class SbBox2f;
class SbBox2d;

class COIN_DLL_API SbBox2s {
public:
  SbBox2s(void);
  SbBox2s(short xmin, short ymin, short xmax, short ymax);
  SbBox2s(const SbVec2s & boxmin, const SbVec2s & boxmax);
  explicit SbBox2s(const SbBox2i32 & box) { setBounds(box); }
  explicit SbBox2s(const SbBox2f & box) { setBounds(box); }
  explicit SbBox2s(const SbBox2d & box) { setBounds(box); }
  ~SbBox2s(void);

  void setBounds(short xmin, short ymin, short xmax, short ymax);
  void setBounds(const SbVec2s & boxmin, const SbVec2s & boxmax);
  SbBox2s & setBounds(const SbBox2i32 & box);
  SbBox2s & setBounds(const SbBox2f & box);
  SbBox2s & setBounds(const SbBox2d & box);

  void getBounds(short & xmin, short & ymin, short & xmax, short & ymax) const;
  void getBounds(SbVec2s & boxmin, SbVec2s & boxmax) const;

  const SbVec2s & getMin(void) const;
  SbVec2s & getMin(void) { return minpt; }
  const SbVec2s & getMax(void) const;
  SbVec2s & getMax(void) { return maxpt; }

  void extendBy(const SbVec2s & point);
  void extendBy(const SbBox2s & box);
  void makeEmpty(void);
  SbBool isEmpty(void) const { return (maxpt[0] < minpt[0]); }
  SbBool hasArea(void) const;

  SbBool intersect(const SbVec2s & point) const;
  SbBool intersect(const SbBox2s & box) const;

  SbVec2f getCenter(void) const { return SbVec2f((minpt[0] + maxpt[0]) * 0.5f, (minpt[1] + maxpt[1]) * 0.5f); }

  void getOrigin(short & originX, short & originY) const;
  void getSize(short & sizeX, short & sizeY) const;
  float getAspectRatio(void) const;
  friend COIN_DLL_API int operator ==(const SbBox2s & b1, const SbBox2s & b2);
  friend COIN_DLL_API int operator !=(const SbBox2s & b1, const SbBox2s & b2);

private:
  SbVec2s minpt, maxpt;

  short width(void) const;
  short height(void) const;
};

COIN_DLL_API int operator ==(const SbBox2s & b1, const SbBox2s & b2);
COIN_DLL_API int operator !=(const SbBox2s & b1, const SbBox2s & b2);

#endif // !COIN_SBBOX2S_H
