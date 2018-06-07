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

#include <Inventor/SbVec3ui32.h>

#include <Inventor/SbVec3i32.h>
#include <Inventor/SbVec3ub.h>
#include <Inventor/SbVec3us.h>

/*!
  \class SbVec3ui32 SbVec3ui32.h Inventor/SbVec3ui32.h

  \brief The SbVec3ui32 class is a 3 dimensional vector with unsigned
  33-bit integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 3 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec2ui32, SbVec4ui32, SbVec3b, SbVec3ub, SbVec3s, SbVec3us, SbVec3i32

  \since Coin-2.5
*/

/*!
  Sets this vector to the 32-bit integer precision vector \a v
  converting the vector to an unsigned 32-bit integer precision vector.
*/
SbVec3ui32 &
SbVec3ui32::setValue(const SbVec3i32 & v)
{
  vec[0] = static_cast<uint32_t>(v[0]);
  vec[1] = static_cast<uint32_t>(v[1]);
  vec[2] = static_cast<uint32_t>(v[2]);
  return *this;
}

/*!
  Sets this vector to the unsigned 8-bit integer precision vector \a v
  converting the vector to an unsigned 32-bit integer precision vector.
*/
SbVec3ui32 &
SbVec3ui32::setValue(const SbVec3ub & v)
{
  vec[0] = static_cast<uint32_t>(v[0]);
  vec[1] = static_cast<uint32_t>(v[1]);
  vec[2] = static_cast<uint32_t>(v[2]);
  return *this;
}

/*!
  Sets this vector to the unsigned short integer precision vector \a v
  converting the vector to an unsigned 32-bit integer precision vector.
*/
SbVec3ui32 &
SbVec3ui32::setValue(const SbVec3us & v)
{
  vec[0] = static_cast<uint32_t>(v[0]);
  vec[1] = static_cast<uint32_t>(v[1]);
  vec[2] = static_cast<uint32_t>(v[2]);
  return *this;
}

/*! 
  Negate the vector (i.e. point it in the opposite direction)
*/
void
SbVec3ui32::negate(void)
{
  vec[0] = static_cast<uint32_t>(0-vec[0]);
  vec[1] = static_cast<uint32_t>(0-vec[1]);
  vec[2] = static_cast<uint32_t>(0-vec[2]);
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/
SbVec3ui32 &
SbVec3ui32::operator *= (double d)
{
  vec[0] = static_cast<uint32_t>(vec[0] * d);
  vec[1] = static_cast<uint32_t>(vec[1] * d);
  vec[2] = static_cast<uint32_t>(vec[2] * d);
  return *this;
}

/*!  
  \fn SbVec3ui32::SbVec3ui32(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/

/*!  
  \fn SbVec3ui32::SbVec3ui32(const uint32_t v[3])
  
  Constructs an SbVec3ui32 instance with initial values from \a v.
*/

/*!  
  \fn SbVec3ui32::SbVec3ui32(uint32_t x, uint32_t y, uint32_t z)

  Constructs an SbVec3ui32 instance with the initial vector endpoint set
  to \a <x,y,z>.
*/

/*!  
  \fn explicit SbVec3ui32::SbVec3ui32(const SbVec3i32 & v)

  Constructs an SbVec3ui32 instance with initial values from the
  32-bit integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec3ui32::SbVec3ui32(const SbVec3ub & v)

  Constructs an SbVec3ui32 instance with initial values from the
  unsigned 8-bit vector \a v.
*/

/*!  
  \fn explicit SbVec3ui32::SbVec3ui32(const SbVec3us & v)
  
  Constructs an SbVec3ui32 instance with initial values from the
  unsigned short integer precision vector \a v.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::setValue(const uint32_t v[3])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::setValue(uint32_t x, uint32_t y, uint32_t z)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const uint32_t * SbVec3ui32::getValue(void) const

  Returns a pointer to an array of three unsigned 32-bit integers
  containing the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec3ui32::getValue(uint32_t & x, uint32_t & y, uint32_t & z) const 
  
  Returns the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn uint32_t & SbVec3ui32::operator [] (int i) 

  Index operator. Returns modifiable x, y or z coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const uint32_t & SbVec3ui32::operator [] (int i) const

  Index operator. Returns non-modifiable x, y or z coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec3ui32::dot(const SbVec3ui32 & v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::operator += (const SbVec3ui32 & v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec3ui32 & SbVec3ui32::operator -= (const SbVec3ui32 & v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec3ui32 SbVec3ui32::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3ui32 instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
