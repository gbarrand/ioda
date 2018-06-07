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
  \class SbVec4d SbVec4d.h Inventor/SbVec4d.h

  \brief The SbVec4d class is a 4 dimensional vector with double
  precision floating point coordinates.

  \ingroup base

  This vector class is not by many other classes in Coin. It provides
  storage for a 3 dimensional homogeneoues vector (with the 4
  components usually referred to as <x, y, z, w>) aswell as simple
  double precision floating point arithmetic operations.

  \sa SbVec2f, SbVec2d, SbVec3f, SbVec3d, SbVec4f
*/

#include <Inventor/SbVec4d.h>

#include <assert.h>

#include <Inventor/SbVec4f.h>
#include <Inventor/SbVec3d.h>
#include <Inventor/C/tidbitsp.h> // coin_debug_normalize()
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG


/*!
  The default constructor does nothing. The vector coordinates will be
  uninitialized until a call the setValue().
*/
SbVec4d::SbVec4d(void)
{
}

/*!
  Constructs an SbVec4d instance with initial values from \a v.
 */
SbVec4d::SbVec4d(const double v[4])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
  this->vec[2] = v[2];
  this->vec[3] = v[3];
}

/*!
  Constructs an SbVec4d instance with the initial homogeneous vector
  set to \a <x,y,z,w>.
 */
SbVec4d::SbVec4d(const double x, const double y, const double z, const double w)
{
  this->vec[0] = x;
  this->vec[1] = y;
  this->vec[2] = z;
  this->vec[3] = w;
}

/*!
  \fn SbVec4d::SbVec4d(const SbVec4f & v)

  Constructs an SbVec4d instance with initial values from the vector
  \a v.

  \since Coin-2.5
*/

/*!
  Calculates and returns the result of taking the dot product of this
  vector and \a v.
 */
double
SbVec4d::dot(const SbVec4d& v) const
{
  return vec[0]*v.vec[0] + vec[1]*v.vec[1] + vec[2]*v.vec[2] + vec[3]*v.vec[3];
}

/*!
  Compares the vector with \a v and returns \c TRUE if the distance
  between the vectors is smaller or equal to the square root of
  \a tolerance.

  The comparison is done in 4D-space, i.e. the \a w component of the
  vector is \e not used to make x, y and z into Cartesian coordinates
  first.
*/
SbBool
SbVec4d::equals(const SbVec4d& v, const double tolerance) const
{
#if COIN_DEBUG
  if(!(tolerance >= 0.0f))
    SoDebugError::postWarning("SbVec4d::equals","Tolerance should be >= 0.0f");
#endif // COIN_DEBUG

  double xdist = this->vec[0] - v[0];
  double ydist = this->vec[1] - v[1];
  double zdist = this->vec[2] - v[2];
  double wdist = this->vec[3] - v[3];

  if((xdist*xdist + ydist*ydist + zdist*zdist + wdist*wdist) <= tolerance)
    return TRUE;
  return FALSE;
}

/*!
  Returns the vector as a Cartesian 3D vector in \a v. This means that
  the 3 first components x, y and z will be divided by the fourth, w.
*/
void
SbVec4d::getReal(SbVec3d & v) const
{
#if COIN_DEBUG
  if(!(this->vec[3] != 0.0f))
    SoDebugError::postWarning("SbVec4d::getReal",
                              "The 4th vector component is zero => "
                              "division by zero");
#endif // COIN_DEBUG

  v[0] = this->vec[0]/this->vec[3];
  v[1] = this->vec[1]/this->vec[3];
  v[2] = this->vec[2]/this->vec[3];
}

/*!
  Returns a pointer to an array of four doubles containing the
  x, y, z and w coordinates of the vector.

  \sa setValue().
 */
const double *
SbVec4d::getValue(void) const
{
  return this->vec;
}

/*!
  Returns the x, y, z and w coordinates of the vector.

  \sa setValue().
 */
void
SbVec4d::getValue(double& x, double& y, double& z, double& w) const
{
  x = this->vec[0];
  y = this->vec[1];
  z = this->vec[2];
  w = this->vec[3];
}

/*!
  Return the length of the vector in 4D space.
 */
double
SbVec4d::length(void) const
{
  return (double)sqrt(vec[0]*vec[0] + vec[1]*vec[1] +
                     vec[2]*vec[2] + vec[3]*vec[3]);
}

/*!
  Return the square of the length of the vector in 4D space.

  \since Coin-2.5
*/

double
SbVec4d::sqrLength(void) const
{
  return vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2] + vec[3]*vec[3];
}

/*!
  Negate the vector.
 */
void
SbVec4d::negate(void)
{
  this->vec[0] = -this->vec[0];
  this->vec[1] = -this->vec[1];
  this->vec[2] = -this->vec[2];
  this->vec[3] = -this->vec[3];
}

/*!
  Normalize the vector to unit length. Return value is the original
  length of the vector before normalization.
 */
double
SbVec4d::normalize(void)
{
  double len = this->length();

  if (len > 0.0) {
    operator/=(len);
  }
#if COIN_DEBUG
  else if (coin_debug_normalize()) {
    SoDebugError::postWarning("SbVec4d::normalize",
                              "The length of the vector should be > 0.0 "
                              "to be able to normalize.");
  }
#endif // COIN_DEBUG
  return len;
}

/*!
  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().
 */
SbVec4d&
SbVec4d::setValue(const double v[4])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
  this->vec[2] = v[2];
  this->vec[3] = v[3];
  return *this;
}

/*!
  Set new coordinates for the vector. Returns reference to self.

  \sa getValue().
 */
SbVec4d&
SbVec4d::setValue(const double x, const double y, const double z, const double w)
{
  this->vec[0] = x;
  this->vec[1] = y;
  this->vec[2] = z;
  this->vec[3] = w;
  return *this;
}

/*!
  Set new value from an SbVec4f instance.
  Returns reference to self.

  \since Coin-2.5
*/

SbVec4d &
SbVec4d::setValue(const SbVec4f & v)
{
  vec[0] = static_cast<double>(v[0]);
  vec[1] = static_cast<double>(v[1]);
  vec[2] = static_cast<double>(v[2]);
  vec[3] = static_cast<double>(v[3]);
  return *this;
}

/*!
  Index operator. Returns modifiable x, y, z or w component of vector.

  \sa getValue() and setValue().
 */
double&
SbVec4d::operator [](const int i)
{
#if COIN_DEBUG
  if(!(i>=0 && i<=3))
    SoDebugError::postWarning("SbVec4d::operator[]",
                              "Index out of bounds [0..3].");
#endif // COIN_DEBUG

  return this->vec[i];
}

/*!
  Index operator. Returns x, y, z or w component of vector.

  \sa getValue() and setValue().
 */
const double&
SbVec4d::operator [](const int i) const
{
#if COIN_DEBUG
  if(!(i>=0 && i<=3))
    SoDebugError::postWarning("SbVec4d::operator[]",
                              "Index out of bounds [0..3].");
#endif // COIN_DEBUG

  return this->vec[i];
}

/*!
  Multiply components of vector with value \a d. Returns reference to self.
 */
SbVec4d&
SbVec4d::operator *=(const double d)
{
  this->vec[0] *= d;
  this->vec[1] *= d;
  this->vec[2] *= d;
  this->vec[3] *= d;
  return *this;
}

/*!
  Divides components of vector with value \a d. Returns reference to self.
 */
SbVec4d&
SbVec4d::operator /=(const double d)
{
#if COIN_DEBUG
  if(!(d != 0.0f))
    SoDebugError::postWarning("SbVec4d::operator/=",
                              "Division by zero.");
#endif // COIN_DEBUG

  // Assumes 1 div and 4 muls is quicker than 4 divs.
  double inv = 1.0f/d;
  this->vec[0] *= inv;
  this->vec[1] *= inv;
  this->vec[2] *= inv;
  this->vec[3] *= inv;
  return *this;
}

/*!
  Adds this vector and vector \a u. Returns reference to self.
 */
SbVec4d&
SbVec4d::operator +=(const SbVec4d& u)
{
  this->vec[0] += u.vec[0];
  this->vec[1] += u.vec[1];
  this->vec[2] += u.vec[2];
  this->vec[3] += u.vec[3];
  return *this;
}

/*!
  Subtracts vector \a u from this vector. Returns reference to self.
 */
SbVec4d&
SbVec4d::operator -=(const SbVec4d& u)
{
  this->vec[0] -= u.vec[0];
  this->vec[1] -= u.vec[1];
  this->vec[2] -= u.vec[2];
  this->vec[3] -= u.vec[3];
  return *this;
}

/*!
  Non-destructive negation operator. Returns a new SbVec4d instance which
  has all components negated.

  \sa negate().
 */
SbVec4d
SbVec4d::operator -(void) const
{
  return SbVec4d(-this->vec[0], -this->vec[1], -this->vec[2], -this->vec[3]);
}

/*!
  \relates SbVec4d

  Returns an SbVec4d instance which is the components of vector \a v
  multiplied with \a d.
 */
SbVec4d
operator *(const SbVec4d& v, const double d)
{
  return SbVec4d(v.vec[0] * d, v.vec[1] * d, v.vec[2] * d, v.vec[3] * d);
}

/*!
  \relates SbVec4d

  Returns an SbVec4d instance which is the components of vector \a v
  multiplied with \a d.
 */
SbVec4d
operator *(const double d, const SbVec4d& v)
{
  return v*d;
}

/*!
  \relates SbVec4d

  Returns an SbVec4d instance which is the components of vector \a v
  divided on the scalar factor \a d.
 */
SbVec4d
operator /(const SbVec4d& v, const double d)
{
#if COIN_DEBUG
  if(!(d != 0.0f))
    SoDebugError::postWarning("SbVec4d::operator/",
                              "Division by zero.");
#endif // COIN_DEBUG

  return SbVec4d(v.vec[0] / d, v.vec[1] / d, v.vec[2] / d, v.vec[3] / d);
}

/*!
  \relates SbVec4d

  Returns an SbVec4d instance which is the sum of vectors \a v1 and \a v2.
 */
SbVec4d
operator +(const SbVec4d& v1, const SbVec4d& v2)
{
  return SbVec4d(v1.vec[0] + v2.vec[0],
                 v1.vec[1] + v2.vec[1],
                 v1.vec[2] + v2.vec[2],
                 v1.vec[3] + v2.vec[3]);
}

/*!
  \relates SbVec4d

  Returns an SbVec4d instance which is vector \a v2 subtracted from
  vector \a v1.
 */
SbVec4d
operator -(const SbVec4d& v1, const SbVec4d& v2)
{
  return SbVec4d(v1.vec[0] - v2.vec[0],
                 v1.vec[1] - v2.vec[1],
                 v1.vec[2] - v2.vec[2],
                 v1.vec[3] - v2.vec[3]);
}

/*!
  \relates SbVec4d

  Returns \a 1 if \a v1 and \a v2 are equal, \a 0 otherwise.

  \sa equals().
 */
int
operator ==(const SbVec4d& v1, const SbVec4d& v2)
{
  if(v1.vec[0] == v2.vec[0] &&
     v1.vec[1] == v2.vec[1] &&
     v1.vec[2] == v2.vec[2] &&
     v1.vec[3] == v2.vec[3]) return TRUE;
  return FALSE;
}

/*!
  \relates SbVec4d

  Returns \a 1 if \a v1 and \a v2 are not equal, \a 0 if they are equal.

  \sa equals().
 */
int
operator !=(const SbVec4d& v1, const SbVec4d& v2)
{
  return !(v1 == v2);
}

/*!
  Dump the state of this object to the \a file stream. Only works in
  debug version of library, method does nothing in an optimized compile.
 */
void
SbVec4d::print(FILE * fp) const
{
#if COIN_DEBUG
  fprintf( fp, "<%f, %f, %f, %f>", this->vec[0], this->vec[1], this->vec[2],
    this->vec[3] );
#endif // COIN_DEBUG
}
