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

#include <Inventor/SbVec2b.h>

#include <Inventor/SbVec2ub.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec2i32.h>

/*!
  \class SbVec2b Inventor/SbVec2b.h
  
  \brief The SbVec2b class is a 2 dimensional vector with 8-bit
  integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 2 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec3b, SbVec4b, SbVec2ub, SbVec2s, SbVec2us, SbVec2i32, SbVec2ui32
  
  \since Coin-2.5
*/

/*!  
  Sets this vector to the unsigned 8-bit integer precision vector
  \a v, converting the vector to an 8-bit integer precision vector
 */
SbVec2b &
SbVec2b::setValue(const SbVec2ub & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an 8-bit integer precision vector.
 */
SbVec2b &
SbVec2b::setValue(const SbVec2s & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the 32-bit integer precision vector \a v,
  converting the vector to an 8-bit integer precision vector.
 */
SbVec2b &
SbVec2b::setValue(const SbVec2i32 & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  return *this;
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
 */
SbVec2b &
SbVec2b::operator *= (double d)
{
  vec[0] = static_cast<int8_t>(vec[0] * d);
  vec[1] = static_cast<int8_t>(vec[1] * d);
  return *this;
}

/*!  
  \fn SbVec2b::SbVec2b(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/


/*!  
  \fn SbVec2b::SbVec2b(const int8_t v[2])

  Constructs an SbVec2b instance with initial values from \a v.
*/

/*!  
  \fn SbVec2b::SbVec2b(int8_t x, int8_t y)

  Constructs an SbVec2b instance with the initial vector endpoint set
  to \a <x,y>.
*/

/*!  
  \fn explicit SbVec2b::SbVec2b(const SbVec2ub & v)

  Constructs an SbVec2b instance with initial values from the unsigned
  8-bit vector \a v.
*/

/*!  
  \fn explicit SbVec2b::SbVec2b(const SbVec2s & v)
  
  Constructs an SbVec2b instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec2b::SbVec2b(const SbVec2i32 & v)

  Constructs an SbVec2b instance with initial values from the 32-bit
  integer precision vector \a v.
*/

/*!  
  \fn SbVec2b & SbVec2b::setValue(const int8_t v[2])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec2b & SbVec2b::setValue(int8_t x, int8_t y)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const int8_t * SbVec2b::getValue(void) const

  Returns a pointer to an array of two 8-bit integers containing the x
  and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec2b::getValue(int8_t & x, int8_t & y) const 
  
  Returns the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn int8_t & SbVec2b::operator [] (int i) 

  Index operator. Returns modifiable x or y coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const int8_t & SbVec2b::operator [] (int i) const

  Index operator. Returns non-modifiable x or y coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec2b::dot(SbVec2b v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn void SbVec2b::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!  
  \fn SbVec2b & SbVec2b::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2b & SbVec2b::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2b & SbVec2b::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec2b & SbVec2b::operator += (SbVec2b v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec2b & SbVec2b::operator -= (SbVec2b v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec2b SbVec2b::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec2b instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
