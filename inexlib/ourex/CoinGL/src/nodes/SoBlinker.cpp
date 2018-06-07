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
  \class SoBlinker SoBlinker.h Inventor/nodes/SoBlinker.h
  \brief The SoBlinker class is a cycling switch node.
  \ingroup nodes

  This switch node cycles its children SoBlinker::speed number of
  times per second. If the node has only one child, it will be cycled
  on and off. Cycling can be turned off using the SoBlinker::on field,
  and the node then behaves like a normal SoSwitch node.

  <b>FILE FORMAT/DEFAULTS:</b>
  \code
    Blinker {
        whichChild -1
        speed 1
        on TRUE
    }
  \endcode
*/

// *************************************************************************

#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoSubNodeP.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/engines/SoTimeCounter.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/sensors/SoOneShotSensor.h>
#include "../misc/PointerMap.h"

// *************************************************************************

/*!
  \var SoSFFloat SoBlinker::speed
  Number of cycles per second.
*/
/*!
  \var SoSFBool SoBlinker::on
  Controls whether cycling is on or off.
*/


// *************************************************************************

SO_NODE_SOURCE(SoBlinker);

// *************************************************************************

#define PRIVATE(_obj_) ((SoBlinkerP*) PointerMap::get(_obj_))

class SoBlinkerP {
public:
  SoBlinkerP(SoBlinker * master) : master(master) { }

  static void whichChildCB(void * closure, SoSensor * sensor) {
    SoBlinkerP * thisp = (SoBlinkerP*) closure;
    thisp->counter->reset.setValue(thisp->whichvalue);

    // if sensor/blinker isn't enabled, we need to manually set the whichChild field
    if (!thisp->counter->on.getValue()) {
      SbBool old = thisp->master->whichChild.enableNotify(FALSE);
      thisp->master->whichChild = thisp->whichvalue;
      thisp->master->whichChild.enableNotify(old);
    }
  }
  SoBlinker * master;
  int whichvalue;
  SoTimeCounter * counter;
  SoOneShotSensor * whichChildSensor;
};

// *************************************************************************

/*!
  Constructor.
*/
SoBlinker::SoBlinker(void)
{
  this->counter = NULL; // this->counter is now obsoleted (pimpl version is used)

  SoBlinkerP * pimpl = new SoBlinkerP(this);
  PointerMap::add(this, pimpl);

  pimpl->counter = new SoTimeCounter;
  pimpl->counter->ref();
  pimpl->counter->min = SO_SWITCH_NONE;
  pimpl->counter->max = SO_SWITCH_NONE;
  pimpl->counter->frequency.connectFrom(&this->speed);
  pimpl->counter->on.connectFrom(&this->on);
  pimpl->whichChildSensor = 
    new SoOneShotSensor(SoBlinkerP::whichChildCB, pimpl);
  pimpl->whichChildSensor->setPriority(1);
  pimpl->whichvalue = SO_SWITCH_NONE;

  SO_NODE_INTERNAL_CONSTRUCTOR(SoBlinker);

  SO_NODE_ADD_FIELD(speed, (1));
  SO_NODE_ADD_FIELD(on, (TRUE));
  
  this->whichChild.connectFrom(&pimpl->counter->output, TRUE);
}

/*!
  Destructor.
*/
SoBlinker::~SoBlinker()
{
  SoBlinkerP * pimpl = PRIVATE(this);
  
  delete pimpl->whichChildSensor;
  pimpl->counter->unref();
  PointerMap::remove(this);
  delete pimpl;
}

// doc in parent
void
SoBlinker::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoBlinker, SO_FROM_INVENTOR_1);
}

// Documented in superclass. Overridden to calculate bbox of all
// children.
void
SoBlinker::getBoundingBox(SoGetBoundingBoxAction * action)
{
  this->whichChild.getValue();
  SoGroup::getBoundingBox(action);
}

// Documented in superclass. Overridden to not write internal engine
// connections.
void
SoBlinker::write(SoWriteAction * action)
{
  SoOutput * out = action->getOutput();

  // Decouple connections to/from internal engine to avoid it being
  // written. (Only done at first pass.)
  if (out->getStage() == SoOutput::COUNT_REFS)
    this->deconnectInternalEngine();

  inherited::write(action);

  // Reenable all connections to/from internal engine. (Only done at
  // last pass.)
  if (out->getStage() == SoOutput::WRITE)
    this->reconnectInternalEngine();
}

// Documented in superclass. Overridden to detect "external" changes
// (i.e. not caused by the internal timer engine).
void
SoBlinker::notify(SoNotList * nl)
{
  SoBlinkerP * pimpl = PRIVATE(this);

  // See if the whichChild field was "manually" set.
  if (nl->getFirstRec()->getBase() == this &&
      nl->getLastField() == &this->whichChild) {
    // delay whichChild reset with the one shot sensor (to enable
    // children to be added before the reset is actually done)

    // disable connection while reading whichChild to get the actual value set
    SbBool old = this->whichChild.isConnectionEnabled();
    this->whichChild.enableConnection(FALSE);
    pimpl->whichvalue = this->whichChild.getValue();
    this->whichChild.enableConnection(old);
    pimpl->whichChildSensor->schedule();
  }

  // Check if a child was added or removed.
  int lastchildidx = this->getNumChildren() - 1;

  if (pimpl->counter->max.getValue() != lastchildidx) {
    // Wrap to avoid recursive invocation.
    pimpl->counter->enableNotify(FALSE);

    // Note that if we have one child, the counting should go from -1
    // to 0 (so the child is toggled on and off).
    pimpl->counter->min.setValue(lastchildidx > 0 ? 0 : SO_SWITCH_NONE);
    pimpl->counter->max.setValue(lastchildidx >= 0 ? lastchildidx : SO_SWITCH_NONE);

    // To avoid SoSwitch getting an out-of-range whichChild value, in
    // case whichChild was at the end.
    if (lastchildidx < this->whichChild.getValue()) {
      pimpl->counter->reset.setValue(lastchildidx);
      this->whichChild.setDirty(TRUE); // Force evaluate() on the field.
    }
    pimpl->counter->enableNotify(TRUE);
  }
  
  inherited::notify(nl);
}

// FIXME: I _think_ we made a mistake when overriding SoNode::copy()
// and making it virtual. The special handling we need below could
// just aswell have been done in an overridden copyContents() method,
// which is the recommended mechanism for application programmers. But
// now I think we'll have to support the "virtual-ity" of
// SoNode::copy(), even though it's confusing to have 2 virtual
// copy-methods (both copy() and copyContents()). 20011220 mortene.

// Overridden to decouple and reconnect engine around copy operation.
SoNode *
SoBlinker::copy(SbBool copyconnections) const
{
  // Decouple connections to/from internal engine to avoid it being
  // copied.
  ((SoBlinker *)this)->deconnectInternalEngine();

  SoBlinker * cp = (SoBlinker *)inherited::copy(copyconnections);

  // Reenable all connections to/from internal engine.
  ((SoBlinker *)this)->reconnectInternalEngine();

  // Need to set this explicitly after reconnect, as the internal
  // engine for the copy initially contains incorrect values. The
  // resulting notification on the copy also sets up correct min and
  // max values for the engine.
  cp->whichChild.setValue(this->whichChild.getValue());

  return cp;
}

// Remove connections to and from internal engine.
void
SoBlinker::deconnectInternalEngine(void)
{
  // Do this first, to avoid field being set due to subsequent engine
  // input value change.
  this->whichChild.disconnect(&PRIVATE(this)->counter->output);

  PRIVATE(this)->counter->on.disconnect(&this->on);
  PRIVATE(this)->counter->on = FALSE;
  PRIVATE(this)->counter->frequency.disconnect(&this->speed);
}


// Reset connections to and from internal engine.
void
SoBlinker::reconnectInternalEngine(void)
{
  PRIVATE(this)->counter->frequency.connectFrom(&this->speed);
  PRIVATE(this)->counter->on.connectFrom(&this->on);

  this->whichChild.connectFrom(&PRIVATE(this)->counter->output, TRUE);
}

#undef PRIVATE
