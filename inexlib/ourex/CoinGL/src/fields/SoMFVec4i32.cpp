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

/*!
  \class SoMFVec4i32 SoMFVec4i32.h Inventor/fields/SoMFVec4i32.h
  \brief The SoMFVec4i32 class is a container for SbVec4i32 vectors.
  \ingroup fields

  This field is used where nodes, engines or other field containers
  needs to store an array of vectors with four elements.

  This field supports application data sharing through a
  setValuesPointer() method. See SoMField documentation for
  information on how to use this function.

  \sa SbVec4i32, SoSFVec4i32
  \since Coin 2.5
*/

// *************************************************************************

#include <Inventor/fields/SoMFVec4i32.h>

#include <assert.h>
#include <Inventor/fields/SoSubFieldP.h>
#include <Inventor/SoInput.h>
#include <Inventor/errors/SoDebugError.h>

#include "shared.h"

// *************************************************************************

SO_MFIELD_SOURCE(SoMFVec4i32, SbVec4i32, const SbVec4i32 &);

SO_MFIELD_SETVALUESPOINTER_SOURCE(SoMFVec4i32, SbVec4i32, SbVec4i32);
SO_MFIELD_SETVALUESPOINTER_SOURCE(SoMFVec4i32, SbVec4i32, int32_t);

// *************************************************************************

// Override from parent class.
void
SoMFVec4i32::initClass(void)
{
  SO_MFIELD_INTERNAL_INIT_CLASS(SoMFVec4i32);
}

// *************************************************************************

// No need to document readValue() and writeValue() here, as the
// necessary information is provided by the documentation of the
// parent classes.
#ifndef DOXYGEN_SKIP_THIS

SbBool
SoMFVec4i32::read1Value(SoInput * in, int idx)
{
  assert(idx < this->maxNum);
  return 
    in->read(this->values[idx][0]) &&
    in->read(this->values[idx][1]) &&
    in->read(this->values[idx][2]) &&
    in->read(this->values[idx][3]);
}

void
SoMFVec4i32::write1Value(SoOutput * out, int idx) const
{
  sosfvec4i32_write_value(out, (*this)[idx]);
}

#endif // DOXYGEN_SKIP_THIS

// *************************************************************************

/*!
  Set \a num vector array elements from \a xyzw, starting at index
  \a start.
*/
void
SoMFVec4i32::setValues(int start, int numarg, const int32_t xyzw[][4])
{
  if(start+numarg > this->maxNum) this->allocValues(start+numarg);
  else if(start+numarg > this->num) this->num = start+numarg;

  for(int i=0; i < numarg; i++) this->values[i+start].setValue(xyzw[i]);
  this->valueChanged();
}

/*!
  Set the vector at \a idx.
*/
void
SoMFVec4i32::set1Value(int idx, int32_t x, int32_t y, int32_t z, int32_t w)
{
  this->set1Value(idx, SbVec4i32(x, y, z, w));
}

/*!
  Set the vector at \a idx.
*/
void
SoMFVec4i32::set1Value(int idx, const int32_t xyzw[4])
{
  this->set1Value(idx, SbVec4i32(xyzw));
}

/*!
  Set this field to contain a single vector with the given
  element values.
*/
void
SoMFVec4i32::setValue(int32_t x, int32_t y, int32_t z, int32_t w)
{
  this->setValue(SbVec4i32(x,y,z,w));
}

/*!
  Set this field to contain a single vector with the given
  element values.
*/
void
SoMFVec4i32::setValue(const int32_t xyzw[4])
{
  if (xyzw == NULL) this->setNum(0);
  else this->setValue(SbVec4i32(xyzw));
}

// *************************************************************************
