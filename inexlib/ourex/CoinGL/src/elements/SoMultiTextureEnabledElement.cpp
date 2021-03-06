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
  \class SoMultiTextureEnabledElement Inventor/elements/SoMultiTextureEnabledElement.h
  \brief The SoMultiTextureEnabledElement class is an element which stores whether texturing is enabled or not.
  \ingroup elements

  \COIN_CLASS_EXTENSION

  \since Coin 2.2
*/


#include <Inventor/elements/SoMultiTextureEnabledElement.h>

#define MAX_UNITS 16

class SoMultiTextureEnabledElementP {
 public:
  SbBool enabled[MAX_UNITS];
  SoMultiTextureEnabledElement::Mode mode[MAX_UNITS];
};

#define PRIVATE(obj) obj->pimpl

SO_ELEMENT_CUSTOM_CONSTRUCTOR_SOURCE(SoMultiTextureEnabledElement);

// doc from parent
void
SoMultiTextureEnabledElement::initClass(void)
{
  SO_ELEMENT_INIT_CLASS(SoMultiTextureEnabledElement, inherited);
}

/*!
  The constructor.
*/
SoMultiTextureEnabledElement::SoMultiTextureEnabledElement(void)
{
  PRIVATE(this) = new SoMultiTextureEnabledElementP;

  this->setTypeId(SoMultiTextureEnabledElement::classTypeId);
  this->setStackIndex(SoMultiTextureEnabledElement::classStackIndex);
}

/*!
  The destructor.
*/
SoMultiTextureEnabledElement::~SoMultiTextureEnabledElement(void)
{
  delete PRIVATE(this);
}

/*!
  Sets the state of this element.
*/
void
SoMultiTextureEnabledElement::set(SoState * state,
                                  SoNode * node,
                                  const int unit,
                                  const SbBool enabled)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    state->getElement(classStackIndex);

  elem->setElt(unit, enabled);
}


// doc from parent
void
SoMultiTextureEnabledElement::init(SoState * state)
{
  for (int i = 0; i < MAX_UNITS; i++) {
    PRIVATE(this)->mode[i] = DISABLED;
    PRIVATE(this)->enabled[i] = FALSE;
  }
}

/*!
  Return current state of this element for \a unit.
*/
SbBool
SoMultiTextureEnabledElement::get(SoState * state, const int unit)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    SoElement::getConstElement(state, classStackIndex);

  assert(unit >= 0 && unit < MAX_UNITS);
  return PRIVATE(elem)->enabled[unit];
}

/*!
  virtual element set function.
*/
void
SoMultiTextureEnabledElement::setElt(const int unit, const SbBool enabled)
{
  assert(unit >= 0 && unit < MAX_UNITS);
  
  Mode mode = (Mode) enabled;
  
  PRIVATE(this)->enabled[unit] = mode != DISABLED; 
  PRIVATE(this)->mode[unit] = mode;
}

/*!
  Returns a pointer to a boolean array. TRUE means unit is enabled and
  that texture coordinates must be sent to the unit. \a lastenabled 
  is set to the last enabled unit.

*/
const SbBool *
SoMultiTextureEnabledElement::getEnabledUnits(SoState * state,
                                              int & lastenabled)
{ 
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    SoElement::getConstElement(state, classStackIndex);

  int i = MAX_UNITS-1;
  while (i >= 0) {
    if (PRIVATE(elem)->enabled[i]) break;
    i--;
  }
  if (i >= 0) {
    lastenabled = i;
    return PRIVATE(elem)->enabled;
  }
  lastenabled = -1;
  return NULL;
}

/*!
  Returns TRUE if unit is enabled (Mode == DISABLED).
*/
SbBool 
SoMultiTextureEnabledElement::isEnabled(const int unit) const
{
  assert(unit >= 0 && unit < MAX_UNITS);
  return PRIVATE(this)->enabled[unit];
}

// doc in parent
void
SoMultiTextureEnabledElement::push(SoState * state)
{
  SoMultiTextureEnabledElement * prev = (SoMultiTextureEnabledElement *)
    this->getNextInStack();

  for (int i = 0; i < MAX_UNITS; i++) {
    PRIVATE(this)->mode[i] = PRIVATE(prev)->mode[i];
    PRIVATE(this)->enabled[i] = PRIVATE(prev)->enabled[i];
  }
}

SbBool
SoMultiTextureEnabledElement::matches(const SoElement * elem) const
{
  SoMultiTextureEnabledElement * e =
    (SoMultiTextureEnabledElement *) elem;
  for (int i = 0; i < MAX_UNITS; i++) {
    if (PRIVATE(e)->mode[i] != PRIVATE(this)->mode[i]) {
      return FALSE;
    }
  }
  return TRUE;
}

SoElement *
SoMultiTextureEnabledElement::copyMatchInfo(void) const
{
  SoMultiTextureEnabledElement * elem =
    (SoMultiTextureEnabledElement *)(getTypeId().createInstance());
  for (int i = 0; i < MAX_UNITS; i++) {
    PRIVATE(elem)->mode[i]= PRIVATE(this)->mode[i];
  }
  return elem;
}

/*!
  Returns the mode of all units. Also returns the last enabled unit
  in \a lastenabled.

  \since Coin 2.5
*/
const SoMultiTextureEnabledElement::Mode * 
SoMultiTextureEnabledElement::getActiveUnits(SoState * state, int & lastenabled)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    SoElement::getConstElement(state, classStackIndex);

  int i = MAX_UNITS-1;
  while (i >= 0) {
    if (PRIVATE(elem)->mode[i] != DISABLED) break;
    i--;
  }
  if (i >= 0) {
    lastenabled = i;
    return PRIVATE(elem)->mode;
  }
  return NULL;
}

/*!
  Enable RECTANGLE texture mode.

  \since Coin 2.5
*/
void 
SoMultiTextureEnabledElement::enableRectangle(SoState * state, 
                                              SoNode * node, 
                                              const int unit)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    state->getElement(classStackIndex);
  // FIXME: in Coin-3, make sure the setElt() method is changed to
  // setElt(const int32_t mode). pederb, 2005-01-31
  elem->setElt(unit, (SbBool) RECTANGLE);
}

/*!
  Enable CUBEMAP texture mode.

  \since Coin 2.5
*/
void 
SoMultiTextureEnabledElement::enableCubeMap(SoState * state, 
                                            SoNode * node, 
                                            const int unit)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    state->getElement(classStackIndex);
  
  // FIXME: in Coin-3, make sure the setElt() method is changed to
  // setElt(const int32_t mode). pederb, 2005-01-31
  elem->setElt(unit, (SbBool) CUBEMAP);
}

/*!

  Disable all active texture units. Convenient when all textures needs
  to be disabled before rendering.
  
  \since Coin 2.5
*/
void 
SoMultiTextureEnabledElement::disableAll(SoState * state)
{
  int lastenabled;
  const SbBool * enabled = getEnabledUnits(state, lastenabled);
  if (enabled) {
    SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
      state->getElement(classStackIndex);
    
    for (int i = 1; i <= lastenabled; i++) {
      if (enabled[i]) {
        elem->setElt(i, FALSE);
      }
    }
  }
}

/*!
  Returns the mode for a texture unit.

  \since Coin 2.5
*/

SoMultiTextureEnabledElement::Mode 
SoMultiTextureEnabledElement::getMode(SoState * state, const int unit)
{
  SoMultiTextureEnabledElement * elem = (SoMultiTextureEnabledElement *)
    SoElement::getConstElement(state, classStackIndex);

  return elem->getMode(unit);
}

SoMultiTextureEnabledElement::Mode 
SoMultiTextureEnabledElement::getMode(const int unit) const
{
  assert(unit >= 0 && unit < MAX_UNITS);
  return PRIVATE(this)->mode[unit];
}


#undef MAX_UNITS
#undef PRIVATE
