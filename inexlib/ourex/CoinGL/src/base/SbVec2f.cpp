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

/*!
  \class SbVec2f SbLinear.h Inventor/SbLinear.h

  \brief The SbVec2f class is a 2 dimensional vector with floating
  point coordinates.

  \ingroup base

  This vector class is used by many other classes in Coin. It provides
  storage for a vector in 2 dimensions aswell as simple floating point
  arithmetic operations on this vector.

  \sa SbVec2d, SbVec3f, SbVec3d, SbVec4f, SbVec4d
*/

#include <Inventor/SbVec2f.h>

#include <assert.h>

#include <Inventor/SbVec2d.h>
#include <Inventor/SbVec2b.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec2i32.h>

#include <Inventor/C/tidbitsp.h> // coin_debug_normalize()
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG


// *************************************************************************

/*!
  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue().
 */

SbVec2f::SbVec2f(void)
{
}

/*!
  Constructs an SbVec2f instance with initial values from \a v.
 */

SbVec2f::SbVec2f(const float v[2])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
}

/*!
  Constructs an SbVec2f instance with the initial vector endpoints from
  \a x and \a y.
 */

SbVec2f::SbVec2f(const float x, const float y)
{
  this->vec[0] = x;
  this->vec[1] = y;
}

/*!
  \fn SbVec2f::SbVec2f(const SbVec2d & v)

  Constructs an SbVec2f instance with initial values from the vector
  \a v.

  \since Coin-2.5
*/

/*!
  \fn SbVec2f::SbVec2f(const SbVec2b & v)

  Constructs an SbVec3f instance with initial values from the vector
  \a v.

  \since Coin-2.5
*/

/*!
  \fn SbVec2f::SbVec2f(const SbVec2s & v)

  Constructs an SbVec3f instance with initial values from the vector
  \a v.

  \since Coin-2.5
*/

/*!
  \fn SbVec2f::SbVec2f(const SbVec2i32 & v)

  Constructs an SbVec3f instance with initial values from the vector
  \a v.

  \since Coin-2.5
*/

/*!
  Calculates and returns the result of taking the dot product of this
  vector and \a v.
 */

float
SbVec2f::dot(const SbVec2f& v) const
{
  return this->vec[0]*v.vec[0] + this->vec[1]*v.vec[1];
}

/*!
  Compares the vector with \a v and returns \c TRUE if the distance
  between the vectors is smaller or equal to the square root of
  \a tolerance.
*/
SbBool
SbVec2f::equals(const SbVec2f& v, const float tolerance) const
{
#if COIN_DEBUG
  if(!(tolerance >= 0.0f))
    SoDebugError::postWarning("SbVec2f::equals",
                              "Tolerance should be >= 0.0f");
#endif // COIN_DEBUG

  float xdist = this->vec[0] - v[0];
  float ydist = this->vec[1] - v[1];

  if((xdist*xdist + ydist*ydist) <= tolerance) return TRUE;
  return FALSE;
}

/*!
  Returns a pointer to an array of two floats containing the x and y
  coordinates of the vector.

  \sa setValue().
 */

const float *
SbVec2f::getValue(void) const
{
  return this->vec;
}

/*!
  Returns the x and y coordinates of the vector.

  \sa setValue().
 */

void
SbVec2f::getValue(float& x, float& y) const
{
  x = this->vec[0];
  y = this->vec[1];
}

/*!
  Return length of vector.
 */

float
SbVec2f::length(void) const
{
  return (float)sqrt(this->vec[0]*this->vec[0] + this->vec[1]*this->vec[1]);
}

/*!
  Returns the square of the length of the vector.

  \since Coin-2.5
*/

float
SbVec2f::sqrLength(void) const
{
  return vec[0] * vec[0] + vec[1] * vec[1];
}

/*!
  Negate the vector (i.e. point it in the opposite direction).
 */

void
SbVec2f::negate(void)
{
  this->vec[0] = -this->vec[0];
  this->vec[1] = -this->vec[1];
}

/*!
  Normalize the vector to unit length. Return value is the original
  length of the vector before normalization.
 */

float
SbVec2f::normalize(void)
{
  float len = this->length();

  if (len > 0.0f) {
    operator/=(len);
  }
#if COIN_DEBUG
  else if (coin_debug_normalize()) {
    SoDebugError::postWarning("SbVec2f::normalize",
                              "The length of the vector should be > 0.0f "
                              "to be able to normalize.");
  }
#endif // COIN_DEBUG
  return len;
}

/*!
  Set new x and y coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().
 */

SbVec2f&
SbVec2f::setValue(const float v[2])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
  return *this;
}

/*!
  Set new x and y coordinates for the vector. Returns reference to self.

  \sa getValue().
 */

SbVec2f&
SbVec2f::setValue(const float x, const float y)
{
  this->vec[0] = x;
  this->vec[1] = y;
  return *this;
}

/*!
  Set new value from an SbVec2d instance.

  \since Coin-2.5
*/

SbVec2f &
SbVec2f::setValue(const SbVec2d & v)
{
  vec[0] = static_cast<float>(v[0]);
  vec[1] = static_cast<float>(v[1]);
  return *this;
}

/*!
  Sets the value from an SbVec2b instance.

  \since Coin-2.5
*/

SbVec2f &
SbVec2f::setValue(const SbVec2b & v)
{
  vec[0] = static_cast<float>(v[0]);
  vec[1] = static_cast<float>(v[1]);
  return *this;
}

/*!
  Sets the value from an SbVec2s instance.

  \since Coin-2.5
*/

SbVec2f &
SbVec2f::setValue(const SbVec2s & v)
{
  vec[0] = static_cast<float>(v[0]);
  vec[1] = static_cast<float>(v[1]);
  return *this;
}

/*!
  Sets the value from an SbVec2i32 instance.

  \since Coin-2.5
*/

SbVec2f &
SbVec2f::setValue(const SbVec2i32 & v)
{
  vec[0] = static_cast<float>(v[0]);
  vec[1] = static_cast<float>(v[1]);
  return *this;
}


/*!
  Index operator. Returns modifiable x or y coordinate.

  \sa getValue() and setValue().
 */

float&
SbVec2f::operator [](const int i)
{
#if COIN_DEBUG
  if(!(i==0 || i==1))
    SoDebugError::postWarning("SbVec2f::operator[]",
                              "Index out of bounds [0,1].");
#endif // COIN_DEBUG
  return this->vec[i];
}

/*!
  Index operator. Returns x or y coordinate.

  \sa getValue().
 */

const float&
SbVec2f::operator [](const int i) const
{
#if COIN_DEBUG
  if(!(i==0 || i==1))
    SoDebugError::postWarning("SbVec2f::operator[]",
                              "Index out of bounds [0,1].");
#endif // COIN_DEBUG
  return this->vec[i];
}

/*!
  Multiply components of vector with value \a d. Returns reference to self.
 */

SbVec2f&
SbVec2f::operator *=(const float d)
{
  this->vec[0] *= d;
  this->vec[1] *= d;
  return *this;
}

/*!
  Divides components of vector with value \a d. Returns reference to self.
 */

SbVec2f&
SbVec2f::operator /=(const float d)
{
#if COIN_DEBUG
  if(!(d != 0.0f))
    SoDebugError::postWarning("SbVec2f::operator/=",
                              "Division by zero.");
#endif // COIN_DEBUG

  this->vec[0] /= d;
  this->vec[1] /= d;
  return *this;
}

/*!
  Adds this vector and vector \a u. Returns reference to self.
 */

SbVec2f&
SbVec2f::operator +=(const SbVec2f& u)
{
  this->vec[0] += u[0];
  this->vec[1] += u[1];
  return *this;
}

/*!
  Subtracts vector \a u from this vector. Returns reference to self.
 */
SbVec2f&
SbVec2f::operator -=(const SbVec2f& u)
{
  this->vec[0] -= u[0];
  this->vec[1] -= u[1];
  return *this;
}

/*!
  Non-destructive negation operator. Returns a new SbVec2f instance which
  points in the opposite direction of this vector.

  \sa negate().
 */

SbVec2f
SbVec2f::operator-(void) const
{
  return SbVec2f(-this->vec[0], -this->vec[1]);
}

/*!
  \relates SbVec2f

  Returns an SbVec2f instance which is the components of vector \a v
  multiplied with \a d.
 */

SbVec2f
operator *(const SbVec2f& v, const float d)
{
  return SbVec2f(v[0]*d, v[1]*d);
}

/*!
  \relates SbVec2f

  Returns an SbVec2f instance which is the components of vector \a v
  multiplied with \a d.
 */

SbVec2f
operator *(const float d, const SbVec2f& v)
{
  return SbVec2f(v[0]*d, v[1]*d);
}

/*!
  \relates SbVec2f

  Returns an SbVec2f instance which is the components of vector \a v
  divided on the scalar factor \a d.
 */

SbVec2f
operator /(const SbVec2f& v, const float d)
{
#if COIN_DEBUG
  if(!(d != 0.0f))
    SoDebugError::postWarning("SbVec2f::operator/",
                              "Division by zero.");
#endif // COIN_DEBUG

  return SbVec2f(v[0]/d, v[1]/d);
}

/*!
  \relates SbVec2f

  Returns an SbVec2f instance which is the sum of vectors \a v1 and \a v2.
 */

SbVec2f
operator +(const SbVec2f& v1, const SbVec2f& v2)
{
  return SbVec2f(v1[0]+v2[0], v1[1]+v2[1]);
}

/*!
  \relates SbVec2f

  Returns an SbVec2f instance which is vector \a v2 subtracted from
  vector \a v1.
 */

SbVec2f
operator -(const SbVec2f& v1, const SbVec2f& v2)
{
  return SbVec2f(v1[0]-v2[0], v1[1]-v2[1]);
}

/*!
  \relates SbVec2f

  Returns \a 1 if \a v1 and \a v2 are equal, \a 0 otherwise.

  \sa equals().
 */
int
operator ==(const SbVec2f& v1, const SbVec2f& v2)
{
  if((v1[0] == v2[0]) && (v1[1] == v2[1])) return TRUE;
  return FALSE;
}

/*!
  \relates SbVec2f

  Returns \a 1 if \a v1 and \a v2 are not equal, \a 0 if they are equal.

  \sa equals().
 */
int
operator !=(const SbVec2f& v1, const SbVec2f& v2)
{
  return !(v1 == v2);
}

/*!
  Dump the state of this object to the \a file stream. Only works in
  debug version of library, method does nothing in an optimized compile.
 */
void
SbVec2f::print(FILE * fp) const
{
#if COIN_DEBUG
  fprintf( fp, "<%f, %f>", this->vec[0], this->vec[1] );
#endif // COIN_DEBUG
}
