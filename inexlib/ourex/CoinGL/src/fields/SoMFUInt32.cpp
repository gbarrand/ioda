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
  \class SoMFUInt32 SoMFUInt32.h Inventor/fields/SoMFUInt32.h
  \brief The SoMFUInt32 class is a container for 32-bit unsigned integer values.
  \ingroup fields

  This field is used where nodes, engines or other field containers
  needs to store a group of multiple 32-bit unsigned integer values.

  This field supports application data sharing through a
  setValuesPointer() method. See SoMField documentation for
  information on how to use this function.

  \sa SoSFUInt32
*/

// *************************************************************************

#include <assert.h>
#include <Inventor/fields/SoMFUInt32.h>
#include <Inventor/fields/SoSubFieldP.h>
#include <Inventor/SoInput.h>
#include <Inventor/errors/SoDebugError.h>

#include "shared.h"

// *************************************************************************

SO_MFIELD_SOURCE_MALLOC(SoMFUInt32, uint32_t, uint32_t);

SO_MFIELD_SETVALUESPOINTER_SOURCE(SoMFUInt32, uint32_t, uint32_t);

// *************************************************************************

// Override from parent class.
void
SoMFUInt32::initClass(void)
{
  SO_MFIELD_INTERNAL_INIT_CLASS(SoMFUInt32);
}

// *************************************************************************

// No need to document readValue() and writeValue() here, as the
// necessary information is provided by the documentation of the
// parent classes.
#ifndef DOXYGEN_SKIP_THIS

SbBool
SoMFUInt32::read1Value(SoInput * in, int idx)
{
  assert(idx < this->maxNum);
  unsigned int tmp;
  if (!in->read(tmp)) return FALSE;
  this->values[idx] = tmp;
  return TRUE;
}

void
SoMFUInt32::write1Value(SoOutput * out, int idx) const
{
  sosfuint32_write_value(out, (*this)[idx]);
}

#endif // DOXYGEN_SKIP_THIS

// *************************************************************************

// Store more than the default single value on each line for ASCII
// format export.
int
SoMFUInt32::getNumValuesPerLine(void) const
{
  return 8;
}

// *************************************************************************
