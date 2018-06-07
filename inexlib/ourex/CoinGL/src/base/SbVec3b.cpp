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

#include <Inventor/SbVec3b.h>

#include <Inventor/SbVec3ub.h>
#include <Inventor/SbVec3s.h>
#include <Inventor/SbVec3i32.h>

/*!
  \class SbVec3b Inventor/SbVec3b.h

  \brief The SbVec3b class is a 3 dimensional vector with 8-bit
  integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 3 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec2b, SbVec4b, SbVec3ub, SbVec3s, SbVec3us, SbVec3i32, SbVec3ui32

  \since Coin-2.5
*/

/*!  
  Sets this vector to the unsigned 8-bit integer precision vector
  \a v, converting the vector to an 8-bit integer precision vector
 */
SbVec3b &
SbVec3b::setValue(const SbVec3ub & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an 8-bit integer precision vector.
 */
SbVec3b &
SbVec3b::setValue(const SbVec3s & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the 32-bit integer precision vector \a v,
  converting the vector to an 8-bit integer precision vector.
 */
SbVec3b &
SbVec3b::setValue(const SbVec3i32 & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  return *this;
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
 */
SbVec3b &
SbVec3b::operator *= (double d)
{
  vec[0] = static_cast<int8_t>(vec[0] * d);
  vec[1] = static_cast<int8_t>(vec[1] * d);
  vec[2] = static_cast<int8_t>(vec[2] * d);
  return *this;
}

/*!  
  \fn SbVec3b::SbVec3b(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/


/*!  
  \fn SbVec3b::SbVec3b(const int8_t v[3])

  Constructs an SbVec3b instance with initial values from \a v.
*/

/*!  
  \fn SbVec3b::SbVec3b(int8_t x, int8_t y, int8_t z)

  Constructs an SbVec3b instance with the initial vector endpoint set
  to \a <x, y, z>.
*/

/*!  
  \fn explicit SbVec3b::SbVec3b(const SbVec3ub & v)

  Constructs an SbVec3b instance with initial values from the unsigned
  8-bit vector \a v.
*/

/*!  
  \fn explicit SbVec3b::SbVec3b(const SbVec3s & v)
  
  Constructs an SbVec3b instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec3b::SbVec3b(const SbVec3i32 & v)

  Constructs an SbVec3b instance with initial values from the 32-bit
  integer precision vector \a v.
*/

/*!  
  \fn SbVec3b & SbVec3b::setValue(const int8_t v[3])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec3b & SbVec3b::setValue(int8_t x, int8_t y, int8_t z)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const int8_t * SbVec3b::getValue(void) const

  Returns a pointer to an array of three 8-bit integers containing the
  x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec3b::getValue(int8_t & x, int8_t & y, int8_t & z) const 
  
  Returns the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn int8_t & SbVec3b::operator [] (int i) 

  Index operator. Returns modifiable x, y or z coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const int8_t & SbVec3b::operator [] (int i) const

  Index operator. Returns non-modifiable x, y or z coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec3b::dot(SbVec3b v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn void SbVec3b::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!  
  \fn SbVec3b & SbVec3b::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3b & SbVec3b::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3b & SbVec3b::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec3b & SbVec3b::operator += (SbVec3b v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec3b & SbVec3b::operator -= (SbVec3b v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec3b SbVec3b::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3b instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
