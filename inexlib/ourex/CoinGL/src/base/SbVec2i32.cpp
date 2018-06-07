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

#include <Inventor/SbVec2i32.h>

#include <limits>
#include <assert.h>

#include <Inventor/SbVec2ui32.h>
#include <Inventor/SbVec2b.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec2d.h>
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

/*!
  \class SbVec2i32 Inventor/SbVec2i32.h

  \brief The SbVec2i32 class is a 2 dimensional vector with 32-bit
  integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 2 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec3i32, SbVec4i32, SbVec2b, SbVec2ub, SbVec2s, SbVec2us, SbVec2ui32


  \since Coin-2.5
*/


/*!  
  Sets this vector to the unsigned 32-bit integer precision vector
  \a v, converting the vector to an 32-bit integer precision vector
 */
SbVec2i32 &
SbVec2i32::setValue(const SbVec2ui32 & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the 8-bit integer precision vector \a v,
  converting the vector to an 32-bit integer precision vector
 */
SbVec2i32 &
SbVec2i32::setValue(const SbVec2b & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an 32-bit integer precision vector
 */
SbVec2i32 &
SbVec2i32::setValue(const SbVec2s & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the floating point precision vector \a v,
  converting the vector to an 32-bit integer precision vector
*/
SbVec2i32 &
SbVec2i32::setValue(const SbVec2f & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<int32_t>::max() || v[0] < -std::numeric_limits<int32_t>::max() ||
      v[1] > std::numeric_limits<int32_t>::max() || v[1] < -std::numeric_limits<int32_t>::max()) {
    SoDebugError::post("SbVec2b::setValue", "SbVec2f argument out of range for SbVec2i32");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  return *this;
}

/*!
  Sets this vector to the double floating point precision vector \a v,
  converting the vector to an 32-bit integer precision vector

  \since Coin-2.5
*/
SbVec2i32 &
SbVec2i32::setValue(const SbVec2d & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<int32_t>::max() || v[0] < -std::numeric_limits<int32_t>::max() ||
      v[1] > std::numeric_limits<int32_t>::max() || v[1] < -std::numeric_limits<int32_t>::max()) {
    SoDebugError::post("SbVec2b::setValue", "SbVec2d argument out of range for SbVec2i32");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  return *this;
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
 */
SbVec2i32 &
SbVec2i32::operator *= (double d)
{
  vec[0] = static_cast<int32_t>(vec[0] * d);
  vec[1] = static_cast<int32_t>(vec[1] * d);
  return *this;
}

/*!  
  \fn SbVec2i32::SbVec2i32(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/


/*!  
  \fn SbVec2i32::SbVec2i32(const int32_t v[2])

  Constructs an SbVec2i32 instance with initial values from \a v.
*/

/*!  
  \fn SbVec2i32::SbVec2i32(int32_t x, int32_t y)

  Constructs an SbVec2i32 instance with the initial vector endpoint set
  to \a <x,y>.
*/

/*!  
  \fn explicit SbVec2i32::SbVec2i32(const SbVec2ui32 & v)

  Constructs an SbVec2i32 instance with initial values from the unsigned
  32-bit integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec2i32::SbVec2i32(const SbVec2s & v)
  
  Constructs an SbVec2i32 instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec2i32::SbVec2i32(const SbVec2b & v)

  Constructs an SbVec2i32 instance with initial values from the 8-bit
  vector \a v.
*/

/*!  
  \fn explicit SbVec2i32::SbVec2i32(const SbVec2f & v)

  Constructs an SbVec2i32 instance with initial values from the
  floating point precision vector \a v.
*/

/*!
  \fn explicit SbVec2i32::SbVec2i32(const SbVec2d & v)
  
  Constructs an SbVec2i32 instance with initial values from the
  double floating point precision vector \a v.
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::setValue(const int32_t v[2])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::setValue(int32_t x, int32_t y)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const int32_t * SbVec2i32::getValue(void) const

  Returns a pointer to an array of two 8-bit integers containing the x
  and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec2i32::getValue(int32_t & x, int32_t & y) const 
  
  Returns the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn int32_t & SbVec2i32::operator [] (int i) 

  Index operator. Returns modifiable x or y coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const int32_t & SbVec2i32::operator [] (int i) const

  Index operator. Returns non-modifiable x or y coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec2i32::dot(const SbVec2i32 & v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn void SbVec2i32::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::operator += (const SbVec2i32 & v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec2i32 & SbVec2i32::operator -= (const SbVec2i32 & v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec2i32 SbVec2i32::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3f instance
  which points in the opposite direction of this vector.

  \sa negate()
*/

