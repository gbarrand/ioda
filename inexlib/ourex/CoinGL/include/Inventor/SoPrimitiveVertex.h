#ifndef COIN_SOPRIMITIVEVERTEX_H
#define COIN_SOPRIMITIVEVERTEX_H

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
#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec4f.h>

class SoDetail;

class COIN_DLL_API SoPrimitiveVertex {
public:
  SoPrimitiveVertex(void);
  SoPrimitiveVertex(const SoPrimitiveVertex & pv);
  ~SoPrimitiveVertex(void);
  const SbVec3f & getPoint(void) const;
  const SbVec3f & getNormal(void) const;
  const SbVec4f & getTextureCoords(void) const;
  int getMaterialIndex(void) const;
  const SoDetail * getDetail(void) const;
  SoPrimitiveVertex & operator = (const SoPrimitiveVertex & pv);

  void setPoint(const SbVec3f & point);
  void setPoint(float x, float y, float z) { point.setValue(x, y, z); }
  void setNormal(const SbVec3f & normal);
  void setNormal(float nx, float ny, float nz) { normal.setValue(nx, ny, nz); }
  void setTextureCoords(const SbVec2f & texcoords);
  void setTextureCoords(float tx, float ty) { textureCoords.setValue(tx, ty, 0.0f, 1.0f); }
  void setTextureCoords(const SbVec3f & texcoords);
  void setTextureCoords(float tx, float ty, float tz) { textureCoords.setValue(tx, ty, tz, 1.0f); }
  void setTextureCoords(const SbVec4f & texcoords);
  void setTextureCoords(float tx, float ty, float tz, float tw) { textureCoords.setValue(tx, ty, tz, tw); }

  void setMaterialIndex(const int index);
  void setDetail(SoDetail * const d);

private:
  SbVec3f point;
  SbVec3f normal;
  SbVec4f textureCoords;
  int materialIndex;
  SoDetail * detail;
};

#endif // !COIN_SOPRIMITIVEVERTEX_H
