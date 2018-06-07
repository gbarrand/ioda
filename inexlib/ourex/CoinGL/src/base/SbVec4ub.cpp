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

#include <Inventor/SbVec4ub.h>

#include <Inventor/SbVec4b.h>
#include <Inventor/SbVec4us.h>
#include <Inventor/SbVec4ui32.h>

/*!
  \class SbVec4ub SbVec4ub.h Inventor/SbVec4ub.h

  \brief The SbVec4ub class is a 4 dimensional vector with unsigned
  8-bit integer coordinates.

  \ingroup base

  This vector class provides storage for a 3 dimensional homogeneoues
  vector (with the 4 components usually referred to as <x, y, z, w>)
  aswell as simple integer precision arithmetic operations.

  \sa SbVec2ub, SbVec3ub, SbVec4b, SbVec4s, SbVec4us, SbVec4i32, SbVec4ui32

  \since Coin-2.5
*/

/*!  
  Sets this vector to the 8-bit integer precision vector \a v,
  converting the vector to a unsigned 8-bit integer precision vector.
*/
SbVec4ub &
SbVec4ub::setValue(const SbVec4b & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  vec[2] = static_cast<uint8_t>(v[2]);
  vec[3] = static_cast<uint8_t>(v[3]);
  return *this;
}

/*!
  Sets this vector to the unsigned short integer precision vector \a
  v, converting the vector to a unsigned 8-bit integer precision vector.
*/
SbVec4ub &
SbVec4ub::setValue(const SbVec4us & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  vec[2] = static_cast<uint8_t>(v[2]);
  vec[3] = static_cast<uint8_t>(v[3]);
  return *this;
}

/*!
  Sets this vector to the unsigned 32-bit integer precision vector \a
  v, converting the vector to a unsigned 8-bit integer precision vector.
*/
SbVec4ub &
SbVec4ub::setValue(const SbVec4ui32 & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  vec[2] = static_cast<uint8_t>(v[2]);
  vec[3] = static_cast<uint8_t>(v[3]);
  return *this;
}

/*!
  Negate the vector (i.e. point it in the opposite direction)
*/
void
SbVec4ub::negate(void)
{
  vec[0] = static_cast<uint8_t>(-vec[0]);
  vec[1] = static_cast<uint8_t>(-vec[1]);
  vec[2] = static_cast<uint8_t>(-vec[2]);
  vec[3] = static_cast<uint8_t>(-vec[3]);
}


/*!  
  Multiply components of vector with value \a d. Returns reference
  to self.
 */
SbVec4ub &
SbVec4ub::operator *= (double d)
{
  vec[0] = static_cast<uint8_t>(vec[0] * d);
  vec[1] = static_cast<uint8_t>(vec[1] * d);
  vec[2] = static_cast<uint8_t>(vec[2] * d);
  vec[3] = static_cast<uint8_t>(vec[3] * d);
  return *this;
}

/*!
  \fn SbVec4ub::SbVec4ub(void)
  
  The default constructor does nothing. The vector coordinates will be
  uninitialized until a call the setValue().
*/

/*!
  \fn SbVec4ub::SbVec4ub(const uint8_t v[4])

  Constructs an SbVec4ub instance with initial values from \a v.
*/

/*!
  \fn SbVec4ub::SbVec4ub(uint8_t x, uint8_t y, uint8_t z, uint8_t w)

  Constructs an SbVec4ub instance with the initial homogeneous vector
  set to \a <x,y,z,w>.
*/

/*!
  \fn explicit SbVec4ub::SbVec4ub(const SbVec4b & v)

  Constructs an SbVec4ub instance with initial values from the 8-bit
  integer precision vector \a v.
*/

/*!
  \fn explicit SbVec4ub::SbVec4ub(const SbVec4us & v)

  Constructs an SbVec4ub instance with initial values from the
  unsigned short integer precision vector \a v.
*/

/*!
  \fn explicit SbVec4ub::SbVec4ub(const SbVec4ui32 & v)

  Constructs an SbVec4ub instance with initial values from the
  unsigned 32-bit integer precision vector \a v.
*/

/*!
  \fn SbVec4ub & SbVec4ub::setValue(const uint8_t v[4]) 
  
  Set new coordinates for the vector from \a v. Returns reference to
  self.
  
  \sa getValue()
*/

/*!
  \fn SbVec4ub & SbVec4ub::setValue(uint8_t x, uint8_t y, uint8_t z, uint8_t w)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!
  \fn const uint8_t * SbVec4ub::getValue(void) const

  Returns a pointer to an array of four uint8_t integers
  containing the x, y, z and w coordinates of the vector.

  \sa setValue()
*/

/*!
  \fn void SbVec4ub::getValue(uint8_t & x, uint8_t & y, uint8_t & z, uint8_t & w) const 

  Returns the x, y, z and w coordinates of the vector.
*/

/*!
  \fn uint8_t & SbVec4ub::operator [] (int i)

  Index operator. Returns modifiable x, y, z or w coordinate of the
  vector.
*/
/*!
  \fn const uint8_t & SbVec4ub::operator [] (int i) const

  Index operator. Returns non-modifiable x, y, z or w coordinate of
  the vector.
*/

/*!
  \fn int32_t SbVec4ub::dot(SbVec4ub v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!
  \fn void SbVec4ub::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!
  \fn SbVec4ub & SbVec4ub::operator *= (int d) 

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4ub & SbVec4ub::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4ub & SbVec4ub::operator /= (double d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4ub & SbVec4ub::operator += (SbVec4ub v) 

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!
  \fn SbVec4ub & SbVec4ub::operator -= (SbVec4ub v) 
  
  Subtracts \a v from this vector. Returns reference to self.
*/
/*!
  \fn SbVec4ub SbVec4ub::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec4ub instance
  which points in the opposite direction of this vector.
  
  \sa negate()
*/
