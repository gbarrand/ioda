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

#include <Inventor/SbVec3us.h>

#include <Inventor/SbVec3s.h>
#include <Inventor/SbVec3ub.h>
#include <Inventor/SbVec3ui32.h>

/*!
  \class SbVec3us SbVec3us.h Inventor/SbVec3us.h

  \brief The SbVec3us class is a 3 dimensional vector with unsigned
  short integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 2 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec2us, SbVec4us, SbVec3b, SbVec3ub, SbVec3s, SbVec3i32, SbVec3ui32

  \since Coin-2.5
*/

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an unsigned short integer precision vector.
 */
SbVec3us &
SbVec3us::setValue(const SbVec3s & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  vec[2] = static_cast<unsigned short>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the unsigned 8-bit integer precision vector
  \a v, converting the vector to an unsigned short integer precision
  vector.
 */
SbVec3us &
SbVec3us::setValue(const SbVec3ub & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  vec[2] = static_cast<unsigned short>(v[2]);
  return *this;
}

/*!  
  Sets this vector to the unsigned 32-bit integer precision vector
  \a v, converting the vector to an unsigned short integer precision
  vector.
 */
SbVec3us &
SbVec3us::setValue(const SbVec3ui32 & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  vec[2] = static_cast<unsigned short>(v[2]);
  return *this;
}

/*! 
  Negate the vector (i.e. point it in the opposite direction)
*/
void
SbVec3us::negate(void)
{
  vec[0] = static_cast<unsigned short>(-vec[0]);
  vec[1] = static_cast<unsigned short>(-vec[1]);
  vec[2] = static_cast<unsigned short>(-vec[2]);
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/
SbVec3us &
SbVec3us::operator *= (double d)
{
  vec[0] = static_cast<unsigned short>(vec[0] * d);
  vec[1] = static_cast<unsigned short>(vec[1] * d);
  vec[2] = static_cast<unsigned short>(vec[2] * d);
  return *this;
}

/*!  
  \fn SbVec3us::SbVec3us(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/

/*!  
  \fn SbVec3us::SbVec3us(const unsigned short v[3])
  
  Constructs an SbVec3us instance with initial values from \a v.
*/

/*!  
  \fn SbVec3us::SbVec3us(unsigned short x, unsigned short y, unsigned short x)

  Constructs an SbVec3us instance with the initial vector endpoint set
  to \a <x, y, z>.
*/

/*!  
  \fn explicit SbVec3us::SbVec3us(const SbVec3s & v)
  
  Constructs an SbVec3us instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec3us::SbVec3us(const SbVec3ub & v)

  Constructs an SbVec3us instance with initial values from the
  unsigned 8-bit vector \a v.
*/

/*!  
  \fn explicit SbVec3us::SbVec3us(const SbVec3ui32 & v)

  Constructs an SbVec3us instance with initial values from the unsigned
  32-bit integer precision vector \a v.
*/

/*!  
  \fn SbVec3us & SbVec3us::setValue(const unsigned short v[3])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec3us & SbVec3us::setValue(unsigned short x, unsigned short y, unsigned short z)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const unsigned short * SbVec3us::getValue(void) const

  Returns a pointer to an array of three unsigned short integers
  containing the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec3us::getValue(unsigned short & x, unsigned short & y, unsigned short & z) const 
  
  Returns the x, y and z coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn unsigned short & SbVec3us::operator [] (int i) 

  Index operator. Returns modifiable x, y or z coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const unsigned short & SbVec3us::operator [] (int i) const

  Index operator. Returns non-modifiable x, y or z coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec3us::dot(const SbVec3us & v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn SbVec3us & SbVec3us::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3us & SbVec3us::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec3us & SbVec3us::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec3us & SbVec3us::operator += (const SbVec3us & v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec3us & SbVec3us::operator -= (const SbVec3us & v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec3us SbVec3us::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec3us instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
