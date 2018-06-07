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

#include <Inventor/SbVec2us.h>

#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec2ub.h>
#include <Inventor/SbVec2ui32.h>

/*!
  \class SbVec2us SbVec2us.h Inventor/SbVec2us.h

  \brief The SbVec2us class is a 2 dimensional vector with unsigned
  short integer coordinates.
  
  \ingroup base
  
  This vector provides storage for a vector in 2 dimensions as well as
  simple integer arithmetic operations.

  \sa SbVec3us, SbVec4us, SbVec2b, SbVec2ub, SbVec2s, SbVec2i32, SbVec2ui32

  \since Coin-2.5
*/

/*!  
  Sets this vector to the short integer precision vector \a v,
  converting the vector to an unsigned short integer precision vector.
 */
SbVec2us &
SbVec2us::setValue(const SbVec2s & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the unsigned 8-bit integer precision vector
  \a v, converting the vector to an unsigned short integer precision
  vector.
 */
SbVec2us &
SbVec2us::setValue(const SbVec2ub & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  return *this;
}

/*!  
  Sets this vector to the unsigned 32-bit integer precision vector
  \a v, converting the vector to an unsigned short integer precision
  vector.
 */
SbVec2us &
SbVec2us::setValue(const SbVec2ui32 & v)
{
  vec[0] = static_cast<unsigned short>(v[0]);
  vec[1] = static_cast<unsigned short>(v[1]);
  return *this;
}

/*! 
  Negate the vector (i.e. point it in the opposite direction)
*/
void
SbVec2us::negate(void)
{
  vec[0] = static_cast<unsigned short>(-vec[0]);
  vec[1] = static_cast<unsigned short>(-vec[1]);
}

/*!
  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/
SbVec2us &
SbVec2us::operator *= (double d)
{
  vec[0] = static_cast<unsigned short>(vec[0] * d);
  vec[1] = static_cast<unsigned short>(vec[1] * d);
  return *this;
}

/*!  
  \fn SbVec2us::SbVec2us(void)

  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
*/

/*!  
  \fn SbVec2us::SbVec2us(const unsigned short v[2])
  
  Constructs an SbVec2us instance with initial values from \a v.
*/

/*!  
  \fn SbVec2us::SbVec2us(unsigned short x, unsigned short y)

  Constructs an SbVec2us instance with the initial vector endpoint set
  to \a <x,y>.
*/

/*!  
  \fn explicit SbVec2us::SbVec2us(const SbVec2s & v)
  
  Constructs an SbVec2us instance with initial values from the short
  integer precision vector \a v.
*/

/*!  
  \fn explicit SbVec2us::SbVec2us(const SbVec2ub & v)

  Constructs an SbVec2us instance with initial values from the
  unsigned 8-bit vector \a v.
*/

/*!  
  \fn explicit SbVec2us::SbVec2us(const SbVec2ui32 & v)

  Constructs an SbVec2us instance with initial values from the unsigned
  32-bit integer precision vector \a v.
*/

/*!  
  \fn SbVec2us & SbVec2us::setValue(const unsigned short v[2])

  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue()
*/

/*!  
  \fn SbVec2us & SbVec2us::setValue(unsigned short x, unsigned short y)

  Set new coordinates for the vector. Returns reference to self.

  \sa getValue()
*/

/*!  
  \fn const unsigned short * SbVec2us::getValue(void) const

  Returns a pointer to an array of two unsigned short integers
  containing the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn void SbVec2us::getValue(unsigned short & x, unsigned short & y) const 
  
  Returns the x and y coordinates of the vector.

  \sa setValue()
*/

/*!  
  \fn unsigned short & SbVec2us::operator [] (int i) 

  Index operator. Returns modifiable x or y coordinate of the vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn const unsigned short & SbVec2us::operator [] (int i) const

  Index operator. Returns non-modifiable x or y coordinate of the
  vector.

  \sa getValue() and setValue()
*/

/*!  
  \fn int32_t SbVec2us::dot(SbVec2us v) const 

  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/

/*!  
  \fn SbVec2us & SbVec2us::operator *= (int d)

  Multiply components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2us & SbVec2us::operator /= (int d) 

  Divides components of vector with scalar value \a d. Returns
  reference to self.
*/

/*!  
  \fn SbVec2us & SbVec2us::operator /= (double d) 

  Divides components of vector with double precision floating point
  value \a d. Returns reference to self.
*/

/*!  
  \fn SbVec2us & SbVec2us::operator += (SbVec2us v)

  Adds this vector and vector \a u. Returns reference to self.
*/

/*!  
  \fn SbVec2us & SbVec2us::operator -= (SbVec2us v)

  Subtracts vector \a u from this vector. Returns reference to self.
*/

/*!  
  \fn SbVec2us SbVec2us::operator - (void) const 

  Non-destructive negation operator. Returns a new SbVec2us instance
  which points in the opposite direction of this vector.

  \sa negate()
*/
