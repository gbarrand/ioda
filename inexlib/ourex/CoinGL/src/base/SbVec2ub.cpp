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

#include <Inventor/SbVec2ub.h>

#include <Inventor/SbVec2b.h>
#include <Inventor/SbVec2us.h>
#include <Inventor/SbVec2ui32.h>

/*!
  \class SbVec2ub Inventor/SbVec2ub.h
  
  \brief The SbVec2ub class is a 2 dimensional vector with unsigned
  8-bit integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 2 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec3ub, SbVec4ub, SbVec2b, SbVec2s, SbVec2us, SbVec2i32, SbVec2ui32
  
  \since Coin-2.5
*/

/*!  
  Sets this vector to the 8-bit integer precision vector \a v
  converting the vector to an unsigned 8-bit integer precision vector.
 */
SbVec2ub &
SbVec2ub::setValue(const SbVec2b & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the unsigned short integer precision vector
  \a v, converting the vector to an unsigned 8-bit integer precision
  vector.
 */
SbVec2ub &
SbVec2ub::setValue(const SbVec2us & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the 32-bit unsigned integer precision vector \a v,
  converting the vector to an unsigned 8-bit integer precision vector.
 */
SbVec2ub &
SbVec2ub::setValue(const SbVec2ui32 & v)
{
  vec[0] = static_cast<uint8_t>(v[0]);
  vec[1] = static_cast<uint8_t>(v[1]);
  return *this;
}

/*! 
  Negate the vector (i.e. point it in the opposite direction)
*/
void
SbVec2ub::negate(void)
{
  vec[0] = static_cast<uint8_t>(-vec[0]);
  vec[1] = static_cast<uint8_t>(-vec[1]);
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/
SbVec2ub &
SbVec2ub::operator *= (double d)
{
  vec[0] = static_cast<uint8_t>(vec[0] * d);
  vec[1] = static_cast<uint8_t>(vec[1] * d);
  return *this;
}

/*!  
  \fn SbVec2ub::SbVec2ub(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/

/*!  
  \fn SbVec2ub::SbVec2ub(const uint8_t v[2])
  
  Constructs an SbVec2ub instance with initial values from \a v.
*/

/*!  
  \fn SbVec2ub::SbVec2ub(uint8_t x, uint8_t y)

  Constructs an SbVec2ub instance with the initial vector endpoint set
  to \a <x,y>.
*/

/*!  
  \fn explicit SbVec2ub::SbVec2ub(const SbVec2b & v)

  Constructs an SbVec2ub instance with initial values from the 8-bit
  vector \a v.
*/

/*!  
  \fn explicit SbVec2ub::SbVec2ub(const SbVec2us & v)
  
  Constructs an SbVec2ub instance with initial values from the
  unsigned short integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec2ub::SbVec2ub(const SbVec2ui32 & v)

  Constructs an SbVec2ub instance with initial values from the unsigned
  32-bit integer precision vector \a v.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::setValue(const uint8_t v[2])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec2ub & SbVec2ub::setValue(uint8_t x, uint8_t y)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const uint8_t * SbVec2ub::getValue(void) const

  Returns a pointer to an array of two unsigned 8-bit integers
  containing the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec2ub::getValue(uint8_t & x, uint8_t & y) const 
  
  Returns the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn uint8_t & SbVec2ub::operator [] (int i) 

  Index operator. Returns modifiable x or y coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const uint8_t & SbVec2ub::operator [] (int i) const

  Index operator. Returns non-modifiable x or y coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec2ub::dot(SbVec2ub v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::operator += (SbVec2ub v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec2ub & SbVec2ub::operator -= (SbVec2ub v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec2ub SbVec2ub::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3f instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
