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
  \class SbVec3s SbLinear.h Inventor/SbLinear.h

  \brief The SbVec3s class is a 3 dimensional vector with short
  integer coordinates.

  \ingroup base

  This vector class provides storage for a 3 dimensional vector as
  well as simple integer arithmetic operations.

  \COIN_CLASS_EXTENSION

  \sa SbVec2s, SbVec4s, SbVec3b, SbVec3ub, SbVec3us, SbVec3i32, SbVec3ui32

  \since Coin 2.0
  \since TGS Inventor ?.?
*/


//
// documentation of inlined methods
//

/*!
  \fn short & SbVec3s::operator[](const int i)

  Index operator. Returns modifiable x, y or z coordinate of vector.

  \sa getValue() and setValue().
*/

/*!
  \fn short SbVec3s::operator[](const int i) const
  Index operator. Returns x, y or z coordinate of vector.

  \sa getValue() and setValue().
*/

#include <Inventor/SbVec3s.h>

#include <limits>
#include <assert.h>

#include <Inventor/SbVec3us.h>
#include <Inventor/SbVec3b.h>
#include <Inventor/SbVec3i32.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbVec3d.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

/*!
  The default constructor does nothing. The vector coordinates will be
  uninitialized until you do a setValue() call.
*/
SbVec3s::SbVec3s(void)
{
}

/*!
  Constructs an SbVec3s instance with initial values from \a v.
*/
SbVec3s::SbVec3s(const short v[3])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
  this->vec[2] = v[2];
}

/*!
  Constructs an SbVec3s instance with the initial vector endpoint set to
  \a <x,y,z>.
*/
SbVec3s::SbVec3s(const short x, const short y, const short z)
{
  this->vec[0] = x;
  this->vec[1] = y;
  this->vec[2] = z;
}

/*!
  \fn SbVec3s::SbVec3s(const SbVec3us & v)

  Sets this vector to the unsigned short integer precision vector \a
  v, converting the vector to an short integer precision vector.

  \since Coin-2.5
*/

/*!
  \fn SbVec3s::SbVec3s(const SbVec3b & v)

  Sets this vector to the 8-bit integer precision vector \a v,
  converting the vector to an short integer precision vector.

  \since Coin-2.5
*/

/*!
  \fn SbVec3s::SbVec3s(const SbVec3i32 & v)

  Sets this vector to the unsigned 32-bit integer precision vector \a
  v, converting the vector to an short integer precision vector.

  \since Coin-2.5
*/

/*!
  \fn SbVec3s::SbVec3s(const SbVec3f & v)

  Sets this vector to the floating point precision vector \a v,
  converting the vector to an short integer precision vector.

  \since Coin-2.5
*/

/*!
  \fn SbVec3s::SbVec3s(const SbVec3d & v)

  Sets this vector to the double floating point precision vector \a v,
  converting the vector to an short integer precision vector.

  \since Coin-2.5
*/

/*!
  Calculates and returns the result of taking the dot product of this
  vector and \a v.
*/
int32_t
SbVec3s::dot(const SbVec3s & v) const
{
  return this->vec[0]*v.vec[0] + this->vec[1]*v.vec[1] + this->vec[2]*v.vec[2];
}

/*!
  Returns a pointer to an array of three shorts containing the x, y
  and z coordinates of the vector.

  \sa setValue().
*/
const short *
SbVec3s::getValue(void) const
{
  return this->vec;
}

/*!
  Returns the x, y and z coordinates of the vector.

  \sa setValue().
*/
void
SbVec3s::getValue(short & x, short & y, short & z) const
{
  x = this->vec[0];
  y = this->vec[1];
  z = this->vec[2];
}

/*!
  Negate the vector (i.e. point it in the opposite direction).
*/
void
SbVec3s::negate(void)
{
  this->vec[0] = -this->vec[0];
  this->vec[1] = -this->vec[1];
  this->vec[2] = -this->vec[2];
}

/*!
  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().
*/
SbVec3s &
SbVec3s::setValue(const short v[3])
{
  this->vec[0] = v[0];
  this->vec[1] = v[1];
  this->vec[2] = v[2];
  return *this;
}

/*!
  Set new coordinates for the vector. Returns reference to self.

  \sa getValue().
*/
SbVec3s &
SbVec3s::setValue(const short x, const short y, const short z)
{
  this->vec[0] = x;
  this->vec[1] = y;
  this->vec[2] = z;
  return *this;
}

/*!
  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().

  \since Coin-2.5
*/

SbVec3s &
SbVec3s::setValue(const SbVec3us & v)
{
  vec[0] = static_cast<short>(v[0]);
  vec[1] = static_cast<short>(v[1]);
  vec[2] = static_cast<short>(v[2]);
  return *this;
}

/*!
  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().

  \since Coin-2.5
*/

SbVec3s &
SbVec3s::setValue(const SbVec3b & v)
{
  vec[0] = static_cast<short>(v[0]);
  vec[1] = static_cast<short>(v[1]);
  vec[2] = static_cast<short>(v[2]);
  return *this;
}

/*!
  Set new coordinates for the vector from \a v. Returns reference to
  self.

  \sa getValue().

  \since Coin-2.5
*/

SbVec3s &
SbVec3s::setValue(const SbVec3i32 & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<short>::max() || v[0] < -std::numeric_limits<short>::max() ||
      v[1] > std::numeric_limits<short>::max() || v[1] < -std::numeric_limits<short>::max() ||
      v[2] > std::numeric_limits<short>::max() || v[2] < -std::numeric_limits<short>::max()) {
    SoDebugError::post("SbVec3s::setValue", "SbVec3i32 argument out of range for SbVec3s");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<short>(v[0]);
  vec[1] = static_cast<short>(v[1]);
  vec[2] = static_cast<short>(v[2]);
  return *this;
}

/*!
  Sets the value from an SbVec3f instance.

  \since Coin-2.5
*/

SbVec3s &
SbVec3s::setValue(const SbVec3f & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<short>::max() || v[0] < -std::numeric_limits<short>::max() ||
      v[1] > std::numeric_limits<short>::max() || v[1] < -std::numeric_limits<short>::max() ||
      v[2] > std::numeric_limits<short>::max() || v[2] < -std::numeric_limits<short>::max()) {
    SoDebugError::post("SbVec3s::setValue", "SbVec3f argument out of range for SbVec3s");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<short>(v[0]);
  vec[1] = static_cast<short>(v[1]);
  vec[2] = static_cast<short>(v[2]);
  return *this;
}

/*!
  Sets the value from an SbVec3d instance.

  \since Coin-2.5
*/

SbVec3s &
SbVec3s::setValue(const SbVec3d & v)
{
#if COIN_DEBUG
  if (v[0] > std::numeric_limits<short>::max() || v[0] < -std::numeric_limits<short>::max() ||
      v[1] > std::numeric_limits<short>::max() || v[1] < -std::numeric_limits<short>::max() ||
      v[2] > std::numeric_limits<short>::max() || v[2] < -std::numeric_limits<short>::max()) {
    SoDebugError::post("SbVec3s::setValue", "SbVec3d argument out of range for SbVec3s");
  }
#endif // COIN_DEBUG
  vec[0] = static_cast<short>(v[0]);
  vec[1] = static_cast<short>(v[1]);
  vec[2] = static_cast<short>(v[2]);
  return *this;
}

/*!
  Multiply components of vector with value \a d. Returns reference to self.
*/
SbVec3s &
SbVec3s::operator *=(int d)
{
  this->vec[0] *= d;
  this->vec[1] *= d;
  this->vec[2] *= d;
  return *this;
}

/*!
  Multiply components of vector with value \a d. Returns reference to self.
 */
SbVec3s&
SbVec3s::operator *=(double d)
{
  this->vec[0] = (short int)(this->vec[0] * d);
  this->vec[1] = (short int)(this->vec[1] * d);
  this->vec[2] = (short int)(this->vec[2] * d);
  return *this;
}

/*!
  Divides components of vector with value \a d. Returns reference to self.
*/
SbVec3s &
SbVec3s::operator /=(int d)
{
#if COIN_DEBUG
  if (!(d != 0.0f))
    SoDebugError::postWarning("SbVec3s::operator/=",
                              "Division by zero.");
#endif // COIN_DEBUG

  this->vec[0] /= d;
  this->vec[1] /= d;
  this->vec[2] /= d;
  return *this;
}

/*!
  Divides components of vector with value \a d. Returns reference to self.
 */
SbVec3s&
SbVec3s::operator /=(double d)
{
#if COIN_DEBUG
  if(!(d != 0.0))
    SoDebugError::postWarning("SbVec3s::operator/=",
                              "Division by zero.");
#endif // COIN_DEBUG

  this->vec[0] = (short int)(this->vec[0] / d);
  this->vec[1] = (short int)(this->vec[1] / d);
  this->vec[2] = (short int)(this->vec[2] / d);
  return *this;
}

/*!
  Adds this vector and vector \a u. Returns reference to self.
*/
SbVec3s &
SbVec3s::operator +=(const SbVec3s & u)
{
  this->vec[0] += u.vec[0];
  this->vec[1] += u.vec[1];
  this->vec[2] += u.vec[2];
  return *this;
}

/*!
  Subtracts vector \a u from this vector. Returns reference to self.
*/
SbVec3s &
SbVec3s::operator -=(const SbVec3s & u)
{
  this->vec[0] -= u.vec[0];
  this->vec[1] -= u.vec[1];
  this->vec[2] -= u.vec[2];
  return *this;
}

/*!
  Non-destructive negation operator. Returns a new SbVec3s instance
  which points in the opposite direction of this vector.

  \sa negate().
*/
SbVec3s
SbVec3s::operator-(void) const
{
  return SbVec3s(-this->vec[0], -this->vec[1], -this->vec[2]);
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  multiplied with \a d.
*/
SbVec3s
operator *(const SbVec3s & v, int d)
{
  return SbVec3s(v.vec[0] * d, v.vec[1] * d, v.vec[2] * d);
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  multiplied with \a d.
 */
SbVec3s
operator *(const SbVec3s& v, double d)
{
  return SbVec3s((short)(v[0]*d), (short)(v[1]*d), (short)(v[2]*d));
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  multiplied with \a d.
*/
SbVec3s
operator *(int d, const SbVec3s & v)
{
  return v * d;
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  multiplied with \a d.
 */
SbVec3s
operator *(double d, const SbVec3s& v)
{
  return SbVec3s((short)(v[0]*d), (short)(v[1]*d), (short)(v[2]*d));
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  divided on the scalar factor \a d.
*/
SbVec3s
operator /(const SbVec3s & v, int d)
{
#if COIN_DEBUG
  if (!(d != 0.0f))
    SoDebugError::postWarning("SbVec3s::operator/",
                              "Division by zero.");
#endif // COIN_DEBUG

  return SbVec3s(v.vec[0] / d, v.vec[1] / d, v.vec[2] / d);
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the components of vector \a v
  divided on \a d.
 */
SbVec3s
operator /(const SbVec3s& v, double d)
{
#if COIN_DEBUG
  if(!(d != 0.0))
    SoDebugError::postWarning("SbVec3s::operator/",
                              "Division by zero.");
#endif // COIN_DEBUG

  return SbVec3s((short)(v[0]/d), (short)(v[1]/d), (short)(v[2]/d));
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is the sum of vectors \a v1 and \a v2.
*/
SbVec3s
operator +(const SbVec3s & v1, const SbVec3s & v2)
{
  return SbVec3s(v1.vec[0] + v2.vec[0],
                 v1.vec[1] + v2.vec[1],
                 v1.vec[2] + v2.vec[2]);
}

/*!
  \relates SbVec3s

  Returns an SbVec3s instance which is vector \a v2 subtracted from
  vector \a v1.
*/
SbVec3s
operator -(const SbVec3s & v1, const SbVec3s & v2)
{
  return SbVec3s(v1.vec[0] - v2.vec[0],
                 v1.vec[1] - v2.vec[1],
                 v1.vec[2] - v2.vec[2]);
}

/*!
  \relates SbVec3s

  Returns \a 1 if \a v1 and \a v2 are \e exactly equal, \a 0 otherwise.
*/
int
operator ==(const SbVec3s & v1, const SbVec3s & v2)
{
  if (v1.vec[0] == v2.vec[0] &&
     v1.vec[1] == v2.vec[1] &&
     v1.vec[2] == v2.vec[2]) return TRUE;
  return FALSE;
}

/*!
  \relates SbVec3s

  Returns \a 1 if \a v1 and \a v2 are not equal, \a 0 if they are equal.
*/
int
operator !=(const SbVec3s & v1, const SbVec3s & v2)
{
  return !(v1 == v2);
}

/*!
  Dump the state of this object to the \a file stream. Only works in
  debug version of library, method does nothing in an optimized
  compile.
*/
void
SbVec3s::print(FILE * fp) const
{
#if COIN_DEBUG
  (void)fprintf(fp, "<%d, %d, %d>", this->vec[0], this->vec[1], this->vec[2]);
#endif // COIN_DEBUG
}
