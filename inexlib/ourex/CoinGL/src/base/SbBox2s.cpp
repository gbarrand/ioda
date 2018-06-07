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
  \class SbBox2s SbBox.h Inventor/SbBox.h
  \brief The SbBox2s class is a 2 dimensional box with short
  integer coordinates.
  \ingroup base

  This box class is used by other classes in Coin for data
  exchange. It provides storage for two box corners with short integer
  coordinates, which is among other things useful for representing
  screen or canvas areas in absolute window coordinates.

  \sa SbBox2d, SbBox2f, SbBox2i32, SbBox3d, SbBox3f, SbBox3i32, SbBox3s, SbXfBox3d, SbXfBox3f
*/

#include <Inventor/SbBox2s.h>

#include <assert.h>
#include <limits.h>
#include <Inventor/SbBox2i32.h>
#include <Inventor/SbBox2f.h>
#include <Inventor/SbBox2d.h>
#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

/*!
  The default constructor makes an empty box.
 */
SbBox2s::SbBox2s(void)
{
  this->makeEmpty();
}

/*!
  Constructs a box with the given corner coordinates.

  \a xmin should be less than \a xmax and \a ymin should be less than
  \a ymax if you want to make a valid box.
 */
SbBox2s::SbBox2s(short xmin, short ymin, short xmax, short ymax)
{
  this->setBounds(xmin, ymin, xmax, ymax);
}

/*!
  Constructs a box with the given corners.

  The coordinates of \a min should be less than the coordinates of
  \a max if you want to make a valid box.
 */
SbBox2s::SbBox2s(const SbVec2s & boxmin, const SbVec2s & boxmax)
{
  this->setBounds(boxmin, boxmax);
}

/*!
  Default destructor does nothing.
 */
SbBox2s::~SbBox2s(void)
{
}

/*!
  \fn SbBox2s::SbBox2s(const SbBox2i32 & box)

  Constructs a box with coordinates from \a box, converting the
  coordinates to short integers

  \since Coin-2.5
*/

/*!
  \fn SbBox2s::SbBox2s(const SbBox2f & box)

  Constructs a box with coordinates from \a box, converting the
  coordinates to short integers

  \since Coin-2.5
*/

/*!
  \fn SbBox2s::SbBox2s(const SbBox2d & box)

  Constructs a box with coordinates from \a box, converting the
  coordinates to short integers

  \since Coin-2.5
*/

/*!
  Reset the boundaries with the boundaries of the given \a box.

  Returns reference to self.

  \since Coin-2.5
*/

SbBox2s &
SbBox2s::setBounds(const SbBox2i32 & box)
{
  if (box.isEmpty()) {
    makeEmpty();
  } else {
    minpt.setValue(box.getMin());
    maxpt.setValue(box.getMax());
  }
  return *this;
}

/*!
  Reset the boundaries with the boundaries of the given \a box.

  Returns reference to self.

  \since Coin-2.5
*/

SbBox2s &
SbBox2s::setBounds(const SbBox2f & box)
{
  if (box.isEmpty()) {
    makeEmpty();
  } else {
    minpt.setValue(box.getMin());
    maxpt.setValue(box.getMax());
  }
  return *this;
}

/*!
  Reset the boundaries with the boundaries of the given \a box.

  Returns reference to self.

  \since Coin-2.5
*/

SbBox2s &
SbBox2s::setBounds(const SbBox2d & box)
{
  if (box.isEmpty()) {
    makeEmpty();
  } else {
    minpt.setValue(box.getMin());
    maxpt.setValue(box.getMax());
  }
  return *this;
}

/*!
  Marks this as an empty box.

  \sa isEmpty().
 */
void
SbBox2s::makeEmpty(void)
{
  this->minpt.setValue(SHRT_MAX, SHRT_MAX);
  this->maxpt.setValue(SHRT_MIN, SHRT_MIN);
}

/*!
  \fn SbBool SbBox2s::isEmpty(void) const

  Check if this has been marked as an empty box.

  \sa makeEmpty().

  \since Coin-2.5
*/

/*!
  Check if the box has "positive" area, i.e. the lower left corner is
  actually lower and more to the left than the other corner point.

  \since Coin-2.5
*/

SbBool
SbBox2s::hasArea(void) const
{
  return (this->minpt[0] < this->maxpt[0] && this->minpt[1] < this->maxpt[1]);
}

/*!
  Returns the minimum point. This should usually be the lower left corner
  point of the box.

  \sa getOrigin(), getMax().
*/
const SbVec2s &
SbBox2s::getMin(void) const
{
  return this->minpt;
}

/*!
  \fn SbVec2s & SbBox2s::getMin(void)

  Returns the lower left corner of the box.
  \since Coin-2.5
*/

/*!
  Returns the maximum point. This should usually be the upper right corner
  point of the box.

  \sa getMin().
*/
const SbVec2s &
SbBox2s::getMax(void) const
{
  return this->maxpt;
}

/*!
  \fn SbVec2s & SbBox2s::getMax(void)

  Returns the upper right corner of the box.

  \sa getMin().

  \since Coin-2.5
*/

/*!
  Extend the boundaries of the box by the given point, i.e. make the
  point fit inside the box if it isn't already within it.
 */
void
SbBox2s::extendBy(const SbVec2s & point)
{
  // The explicit casts are done to humour the HPUX aCC compiler,
  // which will otherwise say ``Template deduction failed to find a
  // match for the call to 'SbMin'''. mortene.
  this->minpt.setValue(SbMin((short)point[0], (short)this->minpt[0]),
                       SbMin((short)point[1], (short)this->minpt[1]));
  this->maxpt.setValue(SbMax((short)point[0], (short)this->maxpt[0]),
                       SbMax((short)point[1], (short)this->maxpt[1]));
}

/*!
  Extend the boundaries of the box by the given \a box parameter. This
  is equal to calling extendBy() twice with the corner points.
 */
void
SbBox2s::extendBy(const SbBox2s & box)
{
#if COIN_DEBUG
  if (box.minpt[0] > box.maxpt[0] || box.minpt[1] > box.maxpt[1])
    SoDebugError::postWarning("SbBox2s::extendBy",
                              "Extending box has negative area.");
#endif // COIN_DEBUG

  this->extendBy(box.getMin());
  this->extendBy(box.getMax());
}

/*!
  Check if the given point lies within the boundaries of this box.
 */
SbBool
SbBox2s::intersect(const SbVec2s & point) const
{
  if((point[0] >= this->minpt[0]) && (point[0] <= this->maxpt[0]) &&
     (point[1] >= this->minpt[1]) && (point[1] <= this->maxpt[1])) return TRUE;
  return FALSE;
}

/*!
  Check if \a box lies wholly or partly within the boundaries
  of this box.
 */
SbBool
SbBox2s::intersect(const SbBox2s & box) const
{
  if((box.getMax()[0] < this->getMin()[0]) ||
     (box.getMax()[1] < this->getMin()[1]) ||
     (box.getMin()[0] > this->getMax()[0]) ||
     (box.getMin()[1] > this->getMax()[1])) return FALSE;
  return TRUE;
}

/*!
  Reset the boundaries of the box.

  \a xmin should be less than \a xmax and \a ymin should be less than
  \a ymax if you want to make a valid box.

  \sa getBounds().
*/
void
SbBox2s::setBounds(short xmin, short ymin, short xmax, short ymax)
{
#if COIN_DEBUG
  if(!(xmin<=xmax && ymin<=ymax))
    SoDebugError::postWarning("SbBox2s::setBounds",
                              "The bounds will give the box negative area.");
#endif // COIN_DEBUG
  this->minpt.setValue(xmin, ymin);
  this->maxpt.setValue(xmax, ymax);
}

/*!
  Reset the boundaries of the box with the given corners.

  The coordinates of \a min should be less than the coordinates of
  \a max if you want to make a valid box.

  \sa getBounds().
 */
void
SbBox2s::setBounds(const SbVec2s & boxmin, const SbVec2s & boxmax)
{
#if COIN_DEBUG
  if(!(boxmin[0]<=boxmax[0] && boxmin[1]<=boxmax[1]))
    SoDebugError::postWarning("SbBox2s::setBounds",
                              "The bounds will give the box negative area.");
#endif // COIN_DEBUG
  this->minpt = boxmin;
  this->maxpt = boxmax;
}

/*!
  Returns the box boundary coordinates.

  \sa setBounds(), getMin(), getMax().
*/
void
SbBox2s::getBounds(short & xmin, short & ymin, short & xmax, short & ymax) const
{
  xmin = this->minpt[0];
  ymin = this->minpt[1];
  xmax = this->maxpt[0];
  ymax = this->maxpt[1];
}

/*!
  Returns the box corner points.

  \sa setBounds(), getMin(), getMax().
*/
void
SbBox2s::getBounds(SbVec2s & boxmin, SbVec2s & boxmax) const
{
  boxmin = this->minpt;
  boxmax = this->maxpt;
}

/*!
  \fn SbVec2f SbBox2s::getCenter(void) const

  Returns the center point of the box.

  \since Coin-2.5
*/

/*!
  Returns the coordinates of the box origin (i.e. the lower left corner).

  \sa getMin().
*/
void
SbBox2s::getOrigin(short & originX, short & originY) const
{
  originX = this->minpt[0];
  originY = this->minpt[1];
}

/*!
  Returns width and height of box.
 */
void
SbBox2s::getSize(short & sizeX, short & sizeY) const
{
#if COIN_DEBUG
  if(!(minpt[0]<=maxpt[0] && minpt[1]<=maxpt[1]))
    SoDebugError::postWarning("SbBox2s::getSize",
                              "The box has negative area.");
#endif // COIN_DEBUG
  sizeX = this->width();
  sizeY = this->height();
}

/*!
  Returns aspect ratio of box, which is defined as box width divided on
  box height.
 */
float
SbBox2s::getAspectRatio(void) const
{
#if COIN_DEBUG
  if(!(minpt[0]<=maxpt[0] && minpt[1]<=maxpt[1]))
    SoDebugError::postWarning("SbBox2s::getAspectRatio",
                              "The box has negative area.");
  if (this->height()==0.0f)
    SoDebugError::postWarning("SbBox2s::getAspectRatio",
                              "The box has zero width=>infinite aspectRatio.");
#endif // COIN_DEBUG
  return ((float)this->width()/(float)this->height());
}

/*!
  \relates SbBox2s

  Check \a b1 and \a b2 for equality.
*/
int
operator ==(const SbBox2s & b1, const SbBox2s & b2)
{
  return b1.getMin()==b2.getMin() && b1.getMax()==b2.getMax();
}

/*!
  \relates SbBox2s

  Check \a b1 and \a b2 for inequality.
*/
int
operator !=(const SbBox2s & b1, const SbBox2s & b2)
{
  return !(b1==b2);
}

/*!
  \COININTERNAL
*/
short
SbBox2s::width(void) const
{
  return this->maxpt[0] - this->minpt[0];
}

/*!
  \COININTERNAL
*/
short
SbBox2s::height(void) const
{
  return this->maxpt[1] - this->minpt[1];
}

