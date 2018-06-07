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

#include <Inventor/SbVec3i32.h>

#include <limits>

#include <Inventor/SbVec3ui32.h>
#include <Inventor/SbVec3b.h>
#include <Inventor/SbVec3s.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec3d.h>
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

/*!  
  \class SbVec3i32 Inventor/SbVec3i32.h 

  \brief The SbVec3i32 class is a 3 dimensional vector with 32-bit
  integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 3 dimensions as well as
  simple integer arithmetic operations.

  \ingroup base

  \sa SbVec2i32, SbVec4i32, SbVec3b, SbVec3ub, SbVec3s, SbVec3us, SbVec3ui32

  \since Coin-2.5
*/

/*!  
  Sets this vector to the unsigned 32-bit integer precision vector
  \a v, converting the vector to an 32-bit integer precision vector
 */
SbVec3i32 &
SbVec3i32::setValue(const SbVec3ui32 & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the 8-bit integer precision vector \a v,
  converting the vector to an 32-bit integer precision vector
 */
SbVec3i32 &
SbVec3i32::setValue(const SbVec3b & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an 32-bit integer precision vector
 */
SbVec3i32 &
SbVec3i32::setValue(const SbVec3s & v)
{
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the floating point precision vector \a v,
  converting the vector to an 32-bit integer precision vector
*/
SbVec3i32 &
SbVec3i32::setValue(const SbVec3f & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<int32_t>::max() || v[0] < -std::numeric_limits<int32_t>::max() ||
      v[1] > std::numeric_limits<int32_t>::max() || v[1] < -std::numeric_limits<int32_t>::max() ||
      v[2] > std::numeric_limits<int32_t>::max() || v[2] < -std::numeric_limits<int32_t>::max()) {
    SoDebugError::post("SbVec3i32::setValue", "SbVec3f argument out of range for SbVec3i32");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

/*!
  Sets this vector to the double floating point precision vector \a v,
  converting the vector to an 32-bit integer precision vector

*/
SbVec3i32 &
SbVec3i32::setValue(const SbVec3d & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<int32_t>::max() || v[0] < -std::numeric_limits<int32_t>::max() ||
      v[1] > std::numeric_limits<int32_t>::max() || v[1] < -std::numeric_limits<int32_t>::max() ||
      v[2] > std::numeric_limits<int32_t>::max() || v[2] < -std::numeric_limits<int32_t>::max()) {
    SoDebugError::post("SbVec3i32::setValue", "SbVec3d argument out of range for SbVec3i32");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<int32_t>(v[0]);
  vec[1] = static_cast<int32_t>(v[1]);
  vec[2] = static_cast<int32_t>(v[2]);
  return *this;
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
 */
SbVec3i32 &
SbVec3i32::operator *= (double d)
{
  vec[0] = static_cast<int32_t>(vec[0] * d);
  vec[1] = static_cast<int32_t>(vec[1] * d);
  vec[2] = static_cast<int32_t>(vec[2] * d);
  return *this;
}

/*!  
  \fn SbVec3i32::SbVec3i32(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/


/*!  
  \fn SbVec3i32::SbVec3i32(const int32_t v[3])

  Constructs an SbVec3i32 instance with initial values from \a v.
*/

/*!  
  \fn SbVec3i32::SbVec3i32(int32_t x, int32_t y, int32_t z)

  Constructs an SbVec3i32 instance with the initial vector endpoint set
  to \a <x,y,z>.
*/

/*!  
  \fn explicit SbVec3i32::SbVec3i32(const SbVec3ui32 & v)

  Constructs an SbVec3i32 instance with initial values from the unsigned
  32-bit integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec3i32::SbVec3i32(const SbVec3s & v)
  
  Constructs an SbVec3i32 instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec3i32::SbVec3i32(const SbVec3b & v)

  Constructs an SbVec3i32 instance with initial values from the 8-bit
  vector \a v.
*/

/*!  
  \fn explicit SbVec3i32::SbVec3i32(const SbVec3f & v)

  Constructs an SbVec3i32 instance with initial values from the
  floating point precision vector \a v.
*/

/*!
  \fn explicit SbVec3i32::SbVec3i32(const SbVec3d & v)
  
  Constructs an SbVec3i32 instance with initial values from the
  double floating point precision vector \a v.
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::setValue(const int32_t v[3])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::setValue(int32_t x, int32_t y, int32_t z)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const int32_t * SbVec3i32::getValue(void) const

  Returns a pointer to an array of three 8-bit integers containing the
  x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec3i32::getValue(int32_t & x, int32_t & y, int32_t & z) const 
  
  Returns the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn int32_t & SbVec3i32::operator [] (int i) 

  Index operator. Returns modifiable x, y or z coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const int32_t & SbVec3i32::operator [] (int i) const

  Index operator. Returns non-modifiable x, y or z coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec3i32::dot(const SbVec3i32 & v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn void SbVec3i32::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::operator += (const SbVec3i32 & v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec3i32 & SbVec3i32::operator -= (const SbVec3i32 & v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec3i32 SbVec3i32::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3f instance
  which points in the opposite direction of this vector.

  \sa negate()
*/

