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

#include <Inventor/SbVec4b.h>

#include <Inventor/SbVec4ub.h>
#include <Inventor/SbVec4s.h>
#include <Inventor/SbVec4i32.h>

/*!
  \class SbVec4b Inventor/SbVec4b.h

  \brief The SbVec4b class is a 4 dimensional vector with 8-bit
  integer coordinates.

  \ingroup base

  This vector class provides storage for a 3 dimensional homogeneoues
  vector (with the 4 components usually referred to as <x, y, z, w>)
  aswell as simple integer precision arithmetic operations.

  \sa SbVec2b, SbVec3b, SbVec4ub, SbVec4s, SbVec4us, SbVec4i32, SbVec4ui32

  \since Coin-2.5
*/

/*!
  Sets this vector to the unsigned 8-bit integer precision vector \a
  v, converting the vector to a 8-bit integer precision vector.
*/
SbVec4b &
SbVec4b::setValue(const SbVec4ub & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  vec[3] = static_cast<int8_t>(v[3]);
  return *this;
}

/*!
  Sets this vector to the short integer precision vector \a
  v, converting the vector to a 8-bit integer precision vector.
*/
SbVec4b &
SbVec4b::setValue(const SbVec4s & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  vec[3] = static_cast<int8_t>(v[3]);
  return *this;
}

/*!
  Sets this vector to the 32-bit integer precision vector \a
  v, converting the vector to a 8-bit integer precision vector.
*/
SbVec4b &
SbVec4b::setValue(const SbVec4i32 & v)
{
  vec[0] = static_cast<int8_t>(v[0]);
  vec[1] = static_cast<int8_t>(v[1]);
  vec[2] = static_cast<int8_t>(v[2]);
  vec[3] = static_cast<int8_t>(v[3]);
  return *this;
}


/*!  
  Multiply components of vector with value \a d. Returns reference
  to self.
 */
SbVec4b &
SbVec4b::operator *= (double d)
{
  vec[0] = static_cast<int8_t>(vec[0] * d);
  vec[1] = static_cast<int8_t>(vec[1] * d);
  vec[2] = static_cast<int8_t>(vec[2] * d);
  vec[3] = static_cast<int8_t>(vec[3] * d);
  return *this;
}

/*!
  \fn SbVec4b::SbVec4b(void)
  
  The default constructor does nothing. The vector coordinates will be
  uninitialized until a call the setValue().
*/

/*!
  \fn SbVec4b::SbVec4b(const int8_t v[4])

  Constructs an SbVec4b instance with initial values from \a v.
*/

/*!
  \fn SbVec4b::SbVec4b(int8_t x, int8_t y, int8_t z, int8_t w)

  Constructs an SbVec4b instance with the initial homogeneous vector
  set to \a <x,y,z,w>.
*/

/*!
  \fn explicit SbVec4b::SbVec4b(const SbVec4ub & v)

  Constructs an SbVec4b instance with initial values from the unsigned
  8-bit integer precision vector \a v.
*/

/*!
  \fn explicit SbVec4b::SbVec4b(const SbVec4s & v)

  Constructs an SbVec4b instance with initial values from the short
  integer precision vector \a v.
*/

/*!
  \fn explicit SbVec4b::SbVec4b(const SbVec4i32 & v)

  Constructs an SbVec4b instance with initial values from the 32-bit
  integer precision vector \a v.
*/

/*!
  \fn SbVec4b & SbVec4b::setValue(const int8_t v[4]) 
  
  Set new coordinates for the vector from \a v. Returns reference to
  self.
  
  \sa getValue()
*/

/*!
  \fn SbVec4b & SbVec4b::setValue(int8_t x, int8_t y, int8_t z, int8_t w)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!
  \fn SbVec4b & SbVec4b::setValue(const SbVec4b & v)

  Constructs an SbVec4b instance with initial values from the vector
  \a v.
*/

/*!
  \fn const int8_t * SbVec4b::getValue(void) const

  Returns a pointer to an array of four int8_t integers
  containing the x, y, z and w coordinates of the vector.

  \sa setValue()
*/

/*!
  \fn void SbVec4b::getValue(int8_t & x, int8_t & y, int8_t & z, int8_t & w) const 

  Returns the x, y, z and w coordinates of the vector.
*/

/*!
  \fn int8_t & SbVec4b::operator [] (int i)

  Index operator. Returns modifiable x, y, z or w coordinate of the
  vector.
*/
/*!
  \fn const int8_t & SbVec4b::operator [] (int i) const

  Index operator. Returns non-modifiable x, y, z or w coordinate of
  the vector.
*/

/*!
  \fn int32_t SbVec4b::dot(SbVec4b v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!
  \fn void SbVec4b::negate(void) 

  Negate the vector (i.e. point it in the opposite direction)
*/

/*!
  \fn SbVec4b & SbVec4b::operator *= (int d) 

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4b & SbVec4b::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4b & SbVec4b::operator /= (double d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!
  \fn SbVec4b & SbVec4b::operator += (SbVec4b v) 

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!
  \fn SbVec4b & SbVec4b::operator -= (SbVec4b v) 
  
  Subtracts \a v from this vector. Returns reference to self.
*/
/*!
  \fn SbVec4b SbVec4b::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec4b instance
  which points in the opposite direction of this vector.
  
  \sa negate()
*/
