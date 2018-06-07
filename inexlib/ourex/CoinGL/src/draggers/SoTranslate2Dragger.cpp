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
  \class SoTranslate2Dragger SoTranslate2Dragger.h Inventor/draggers/SoTranslate2Dragger.h
  \brief The SoTranslate2Dragger class provides a mechanism for the end-user to translate in a plane.
  \ingroup draggers

  \DRAGGER_DEFAULT_SCREENSHOT

  <center>
  <img src="http://doc.coin3d.org/images/Coin/draggers/translate2.png">
  </center>

  Use this dragger to allow the end-user of your application to
  translate along the X-axis and the Y-axis at the same time, ie
  freely inside a 3D plane. (Use a transformation node in front of the
  dragger to position it and re-orient it to translate in any plane.)

  By holding down a \c SHIFT key, the end-user can temporarily
  constrain the dragger to a single one of the two axes.

  \sa SoTranslate1Dragger, SoDragPointDragger
*/

#include <string.h>

#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/nodekits/SoSubKitP.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <data/draggerDefaults/translate2Dragger.h>

#define CONSTRAINT_OFF  0
#define CONSTRAINT_WAIT 1
#define CONSTRAINT_X    2
#define CONSTRAINT_Y    3

/*!
  \var SoSFVec3f SoTranslate2Dragger::translation

  Continuously updated to contain the current translation from the
  dragger's local origo position. Only the first two components (the X
  and Y values) can be changed by the end-user.

  The application programmer applying this dragger in his code should
  connect the relevant node fields in the scene to this field to make
  it follow the dragger.
*/

/*!
  \var SbPlaneProjector * SoTranslate2Dragger::planeProj

  The SbPlaneProjector instance used for projecting from 2D mouse
  cursor positions to 3D points.
*/

/*!
  \var SoFieldSensor * SoTranslate2Dragger::fieldSensor
  \COININTERNAL
*/
/*!
  \var SbVec3f SoTranslate2Dragger::worldRestartPt
  \COININTERNAL
*/

#ifndef DOXYGEN_SKIP_THIS // Don't document internal classes.

class SoTranslate2DraggerP {
public:
  SbVec3f lastmotion;
  SbVec3f extramotion;
};

#endif // DOXYGEN_SKIP_THIS

SO_KIT_SOURCE(SoTranslate2Dragger);

#define THIS this->pimpl
#define THISP thisp->pimpl

// doc in superclass
void
SoTranslate2Dragger::initClass(void)
{
  SO_KIT_INTERNAL_INIT_CLASS(SoTranslate2Dragger, SO_FROM_INVENTOR_1);
}

// FIXME: document which parts need to be present in the geometry
// scenegraph, and what role they play in the dragger. 20010913 mortene.
/*!
  \DRAGGER_CONSTRUCTOR

  \NODEKIT_PRE_DIAGRAM

  \verbatim
  CLASS SoTranslate2Dragger
  -->"this"
        "callbackList"
        "topSeparator"
           "motionMatrix"
           "geomSeparator"
  -->         "translatorSwitch"
  -->            "translator"
  -->            "translatorActive"
  -->         "feedbackSwitch"
  -->            "feedback"
  -->            "feedbackActive"
  -->         "axisFeedbackSwitch"
  -->            "xAxisFeedback"
  -->            "yAxisFeedback"
  
  \endverbatim

  \NODEKIT_POST_DIAGRAM


  \NODEKIT_PRE_TABLE

  \verbatim
  CLASS SoTranslate2Dragger
  PVT   "this",  SoTranslate2Dragger  --- 
        "callbackList",  SoNodeKitListPart [ SoCallback, SoEventCallback ] 
  PVT   "topSeparator",  SoSeparator  --- 
  PVT   "motionMatrix",  SoMatrixTransform  --- 
  PVT   "geomSeparator",  SoSeparator  --- 
  PVT   "translatorSwitch",  SoSwitch  --- 
        "translator",  SoSeparator  --- 
        "translatorActive",  SoSeparator  --- 
  PVT   "feedbackSwitch",  SoSwitch  --- 
        "feedback",  SoSeparator  --- 
        "feedbackActive",  SoSeparator  --- 
  PVT   "axisFeedbackSwitch",  SoSwitch  --- 
        "xAxisFeedback",  SoSeparator  --- 
        "yAxisFeedback",  SoSeparator  --- 
  \endverbatim

  \NODEKIT_POST_TABLE
*/
SoTranslate2Dragger::SoTranslate2Dragger(void)
  : pimpl(new SoTranslate2DraggerP)
{
  SO_KIT_INTERNAL_CONSTRUCTOR(SoTranslate2Dragger);

  SO_KIT_ADD_CATALOG_ENTRY(translatorSwitch, SoSwitch, TRUE, geomSeparator, feedbackSwitch, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(translator, SoSeparator, TRUE, translatorSwitch, translatorActive, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(translatorActive, SoSeparator, TRUE, translatorSwitch, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE, geomSeparator, axisFeedbackSwitch, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE, feedbackSwitch, feedbackActive, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE, feedbackSwitch, "", TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(axisFeedbackSwitch, SoSwitch, TRUE, geomSeparator, "", FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxisFeedback, SoSeparator, TRUE, axisFeedbackSwitch, yAxisFeedback, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxisFeedback, SoSeparator, TRUE, axisFeedbackSwitch, "", TRUE);

  if (SO_KIT_IS_FIRST_INSTANCE()) {
    SoInteractionKit::readDefaultParts("translate2Dragger.iv",
                                       TRANSLATE2DRAGGER_draggergeometry,
                                       (int)strlen(TRANSLATE2DRAGGER_draggergeometry));
  }
  SO_KIT_ADD_FIELD(translation, (0.0f, 0.0f, 0.0f));
  SO_KIT_INIT_INSTANCE();

  // initialize default parts
  this->setPartAsDefault("translator", "translate2Translator");
  this->setPartAsDefault("translatorActive", "translate2TranslatorActive");
  this->setPartAsDefault("feedback", "translate2Feedback");
  this->setPartAsDefault("feedbackActive", "translate2FeedbackActive");
  this->setPartAsDefault("xAxisFeedback", "translate2XAxisFeedback");
  this->setPartAsDefault("yAxisFeedback", "translate2YAxisFeedback");

  // initialize swich values
  SoSwitch *sw;
  sw = SO_GET_ANY_PART(this, "translatorSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 0);
  sw = SO_GET_ANY_PART(this, "feedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 0);
  sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, SO_SWITCH_NONE);

  // setup projector
  this->planeProj = new SbPlaneProjector();
  this->addStartCallback(SoTranslate2Dragger::startCB);
  this->addMotionCallback(SoTranslate2Dragger::motionCB);
  this->addFinishCallback(SoTranslate2Dragger::finishCB);
  this->addOtherEventCallback(SoTranslate2Dragger::metaKeyChangeCB);
  this->addValueChangedCallback(SoTranslate2Dragger::valueChangedCB);

  this->fieldSensor = new SoFieldSensor(SoTranslate2Dragger::fieldSensorCB, this);
  this->fieldSensor->setPriority(0);

  this->constraintState = CONSTRAINT_OFF;

  this->setUpConnections(TRUE, TRUE);
}

/*!
  Protected destructor.

  (Dragger classes are derived from SoBase, so they are reference
  counted and automatically destroyed when their reference count goes
  to 0.)
 */
SoTranslate2Dragger::~SoTranslate2Dragger()
{
  delete this->planeProj;
  delete this->fieldSensor;
  delete this->pimpl;
}

// doc in superclass
SbBool
SoTranslate2Dragger::setUpConnections(SbBool onoff, SbBool doitalways)
{
  if (!doitalways && this->connectionsSetUp == onoff) return onoff;

  SbBool oldval = this->connectionsSetUp;

  if (onoff) {
    inherited::setUpConnections(onoff, doitalways);

    SoTranslate2Dragger::fieldSensorCB(this, NULL);

    if (this->fieldSensor->getAttachedField() != &this->translation) {
      this->fieldSensor->attach(&this->translation);
    }
  }
  else {
    if (this->fieldSensor->getAttachedField() != NULL) {
      this->fieldSensor->detach();
    }
    inherited::setUpConnections(onoff, doitalways);
  }
  this->connectionsSetUp = onoff;
  return oldval;
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::fieldSensorCB(void * d, SoSensor * s)
{
  assert(d);
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  SbMatrix matrix = thisp->getMotionMatrix();
  SbVec3f t = thisp->translation.getValue();
  matrix[3][0] = t[0];
  matrix[3][1] = t[1];
  matrix[3][2] = t[2];
  thisp->setMotionMatrix(matrix);
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::valueChangedCB(void *, SoDragger * d)
{
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  SbMatrix matrix = thisp->getMotionMatrix();

  SbVec3f t;
  t[0] = matrix[3][0];
  t[1] = matrix[3][1];
  t[2] = matrix[3][2];

  thisp->fieldSensor->detach();
  if (thisp->translation.getValue() != t) {
    thisp->translation = t;
  }
  thisp->fieldSensor->attach(&thisp->translation);
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::startCB(void *, SoDragger * d)
{
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  thisp->dragStart();
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::motionCB(void *, SoDragger * d)
{
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  thisp->drag();
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::finishCB(void *, SoDragger * d)
{
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  thisp->dragFinish();
}

/*! \COININTERNAL */
void
SoTranslate2Dragger::metaKeyChangeCB(void *, SoDragger *d)
{
  SoTranslate2Dragger *thisp = (SoTranslate2Dragger*)d;
  if (!thisp->isActive.getValue()) return;

  const SoEvent *event = thisp->getEvent();
  if (thisp->constraintState == CONSTRAINT_OFF &&
      event->wasShiftDown()) thisp->drag();
  else if (thisp->constraintState != CONSTRAINT_OFF &&
           !event->wasShiftDown()) thisp->drag();
}

/*! \COININTERNAL
  Called when dragger is selected (picked) by the user.
*/
void
SoTranslate2Dragger::dragStart(void)
{
  SoSwitch *sw;
  sw = SO_GET_ANY_PART(this, "translatorSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 1);
  sw = SO_GET_ANY_PART(this, "feedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 1);
  sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, SO_SWITCH_ALL);

  SbVec3f hitPt = this->getLocalStartingPoint();
  this->planeProj->setPlane(SbPlane(SbVec3f(0.0f, 0.0f, 1.0f), hitPt));
  if (this->getEvent()->wasShiftDown()) {
    this->getLocalToWorldMatrix().multVecMatrix(hitPt, this->worldRestartPt);
    this->constraintState = CONSTRAINT_WAIT;
  }

  THIS->extramotion = SbVec3f(0, 0, 0);
}

/*! \COININTERNAL
  Called when user drags the mouse after picking the dragger.
*/
void
SoTranslate2Dragger::drag(void)
{
  this->planeProj->setViewVolume(this->getViewVolume());
  this->planeProj->setWorkingSpace(this->getLocalToWorldMatrix());

  SbVec3f projPt = this->planeProj->project(this->getNormalizedLocaterPosition());

  const SoEvent *event = this->getEvent();
  if (event->wasShiftDown() && this->constraintState == CONSTRAINT_OFF) {
    this->constraintState = CONSTRAINT_WAIT;
    this->setStartLocaterPosition(event->getPosition());
    this->getLocalToWorldMatrix().multVecMatrix(projPt, this->worldRestartPt);
  }
  else if (!event->wasShiftDown() && this->constraintState != CONSTRAINT_OFF) {
    SbVec3f worldProjPt;
    this->getLocalToWorldMatrix().multVecMatrix(projPt, worldProjPt);
    this->setStartingPoint(worldProjPt);
    THIS->extramotion += THIS->lastmotion;
    
    SoSwitch *sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
    SoInteractionKit::setSwitchValue(sw, SO_SWITCH_ALL);
    this->constraintState = CONSTRAINT_OFF;
  }

  SbVec3f startPt = this->getLocalStartingPoint();
  SbVec3f motion;
  SbVec3f localrestartpt;

  if (this->constraintState != CONSTRAINT_OFF) {
    this->getWorldToLocalMatrix().multVecMatrix(this->worldRestartPt,
                                                localrestartpt);
    motion = localrestartpt - startPt;
  }
  else motion = projPt - startPt;

  switch(this->constraintState) {
  case CONSTRAINT_OFF:
    break;
  case CONSTRAINT_WAIT:
    if (this->isAdequateConstraintMotion()) {
      SbVec3f newmotion = projPt - localrestartpt;
      if (fabs(newmotion[0]) >= fabs(newmotion[1])) {
        this->constraintState = CONSTRAINT_X;
        motion[0] += newmotion[0];
        SoSwitch *sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
        SoInteractionKit::setSwitchValue(sw, 0);
      }
      else {
        this->constraintState = CONSTRAINT_Y;
        motion[1] += newmotion[1];
        SoSwitch *sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
        SoInteractionKit::setSwitchValue(sw, 1);
      }
    }
    else {
      return;
    }
    break;
  case CONSTRAINT_X:
    motion[0] += projPt[0] - localrestartpt[0];
    break;
  case CONSTRAINT_Y:
    motion[1] += projPt[1] - localrestartpt[1];
    break;
  }
  THIS->lastmotion = motion;
  this->setMotionMatrix(this->appendTranslation(this->getStartMotionMatrix(),
                                                THIS->extramotion+motion));
}

/*! \COININTERNAL
  Called when mouse button is released after picking and interacting
  with the dragger.
*/
void
SoTranslate2Dragger::dragFinish(void)
{
  SoSwitch *sw;
  sw = SO_GET_ANY_PART(this, "translatorSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 0);
  sw = SO_GET_ANY_PART(this, "feedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, 0);
  sw = SO_GET_ANY_PART(this, "axisFeedbackSwitch", SoSwitch);
  SoInteractionKit::setSwitchValue(sw, SO_SWITCH_NONE);
  this->constraintState = CONSTRAINT_OFF;
}
