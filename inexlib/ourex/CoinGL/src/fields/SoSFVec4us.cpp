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
  \class SoSFVec4us SoSFVec4us.h Inventor/fields/SoSFVec4us.h
  \brief The SoSFVec4us class is a container for an SbVec4us vector.
  \ingroup fields

  This field is used where nodes, engines or other field containers
  needs to store a single vector with four elements.

  \sa SbVec4us, SoMFVec4us
  \since Coin 2.5
*/

// *************************************************************************

#include <Inventor/fields/SoSFVec4us.h>

#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSubFieldP.h>

#include "shared.h"

// *************************************************************************

SO_SFIELD_SOURCE(SoSFVec4us, SbVec4us, const SbVec4us &);

// *************************************************************************

// Override from parent class.
void
SoSFVec4us::initClass(void)
{
  SO_SFIELD_INTERNAL_INIT_CLASS(SoSFVec4us);
}

// *************************************************************************

// No need to document readValue() and writeValue() here, as the
// necessary information is provided by the documentation of the
// parent classes.
#ifndef DOXYGEN_SKIP_THIS

SbBool
SoSFVec4us::readValue(SoInput * in)
{
  return 
    in->read(this->value[0]) && 
    in->read(this->value[1]) &&
    in->read(this->value[2]) &&
    in->read(this->value[3]);
}

void
SoSFVec4us::writeValue(SoOutput * out) const
{
  sosfvec4us_write_value(out, this->getValue());
}

#endif // DOXYGEN_SKIP_THIS


/*!
  Set value of vector.
*/
void
SoSFVec4us::setValue(unsigned short x, unsigned short y, unsigned short z, unsigned short w)
{
  this->setValue(SbVec4us(x, y, z, w));
}

/*!
  Set value of vector.
*/
void
SoSFVec4us::setValue(const unsigned short xyzw[4])
{
  this->setValue(SbVec4us(xyzw));
}
