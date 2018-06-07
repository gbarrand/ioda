/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoArrow                                                 */
/* Description:      Represents an Arrow, or Vector or something             */
/* Author:           Joe Boudreau September 1997                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
// this :
#include <HEPVis/nodes/SoArrow.h>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoScale.h>

#include <HEPVis/SbMath.h>

#include <assert.h>

// This statement is required
SO_NODE_SOURCE(SoArrow)

// Constructor
SoArrow::SoArrow() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoArrow);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(tail,                (0,0,0));
  SO_NODE_ADD_FIELD(tip,                 (0,0,1));
  SO_NODE_ADD_FIELD(size,                (0.02F) );
  SO_NODE_ADD_FIELD(conicalShaft,        (FALSE));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  children = new SoChildList(this);

  cachedTip.setValue(0,0,0);
  cachedTail.setValue(0,0,0);
}

// Destructor
SoArrow::~SoArrow() {
  delete children;
}


// initClass
void SoArrow::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoArrow,SoShape,"Shape");
}


// GLRrender
void SoArrow::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoArrow::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoArrow::getChildren() const {
  return children;
}


// computeBBox
void SoArrow::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  SbVec3f mtip=tip.getValue(),
          mtail=tail.getValue();
  float size=(mtip-mtail).length();
  
  float xmin,ymin,zmin,xmax,ymax,zmax;

  if (mtip[0]>mtail[0]) {
    xmax =mtip [0]+0.2F*size;
    xmin =mtail[0]-0.2F*size;
  }
  else {
    xmax =mtail[0]+0.2F*size;
    xmin =mtip [0]-0.2F*size;
  }

  if (mtip[1]>mtail[1]) {
    ymax =mtip [1]+0.2F*size;
    ymin =mtail[1]-0.2F*size;
  }
  else {
    ymax =mtail[1]+0.2F*size;
    ymin =mtip [1]-0.2F*size;
  }

  if (mtip[2]>mtail[2]) {
    zmax =mtip [2]+0.2F*size;
    zmin =mtail[2]-0.2F*size;
  }
  else {
    zmax =mtail[2]+0.2F*size;
    zmin =mtip [2]-0.2F*size;
  }

  SbVec3f MMin(xmin,ymin,zmin), MMax(xmax,ymax,zmax);
  center.setValue((xmax+xmin)/2,(ymax+ymin)/2,(zmax+zmin)/2);
  box.setBounds(MMin,MMax);
}




// updateChildren
void SoArrow::updateChildren() {
  assert(children->getLength()==1);

  if( cachedTip != tip.getValue() || cachedTail != tail.getValue() || 
      cachedSize != size || cachedConicalShaft != conicalShaft ) {

    // Redraw the Arrow....
    
    SoSeparator       *axis                = (SoSeparator *)  ( *children)[0];
    SoTranslation     *finalTranslation    = (SoTranslation *)( axis->getChild(0));
    SoScale           *scale               = (SoScale *      )( axis->getChild(1));
    SoRotation        *myRotation          = (SoRotation *   )( axis->getChild(2));
    SoRotationXYZ     *rot                 = (SoRotationXYZ *)( axis->getChild(3));
    SoTranslation     *zTranslation        = (SoTranslation *)( axis->getChild(4));
    SoTranslation     *cTranslation        = (SoTranslation *)( axis->getChild(6));
    SoCone            *axisCone            = (SoCone *       )( axis->getChild(7));
    
    
    float length = (tip.getValue()-tail.getValue()).length();
    finalTranslation->translation.setValue(tail.getValue());
    scale->scaleFactor.setValue(length/2,length/2,length/2);
    SbVec3f ax = SbVec3f(0,0,1).cross(tip.getValue()-tail.getValue());
    float an = FASIN(ax.length()/length);
    myRotation->rotation.setValue(ax,an);
    zTranslation->translation.setValue(0,1,0);
    rot->axis=SoRotationXYZ::X;
    rot->angle = FM_PI_2;

    float arrowsize = size.getValue();
    if ( conicalShaft.getValue() ) {
      SoSeparator* shaft = (SoSeparator*) axis->getChild(5);
      SoRotationXYZ* rot2 = (SoRotationXYZ*) (shaft->getChild(0));
      rot2->axis.setValue(SoRotationXYZ::X);
      rot2->angle.setValue(FM_PI);
      SoCone* axisCone = (SoCone*) (shaft->getChild(1));
      axisCone->bottomRadius.setValue( 2 * arrowsize );
    }
    else {
      SoCylinder* axisCyl  = (SoCylinder *   )( axis->getChild(5));
      axisCyl->radius.setValue( 2 * arrowsize );
    }
    cTranslation->translation.setValue(0,1,0);
    axisCone->bottomRadius.setValue( arrowsize * 4.0F );
    axisCone->height.setValue( arrowsize * 8.0F );
  }
  cachedTip=tip.getValue();
  cachedTail=tail.getValue();
}

// generateChildren
void SoArrow::generateChildren() {
  assert(children->getLength() ==0);
  SoSeparator   *axis=new SoSeparator();
  SoTranslation *finalTranslation=new SoTranslation;
  SoScale       *scale=new SoScale();
  SoRotation *myRotation = new SoRotation();
  SoRotationXYZ  *rot = new SoRotationXYZ;
  SoTranslation  *zTranslation=new SoTranslation;
  SoTranslation  *cTranslation=new SoTranslation;
  SoCone     *axisCone=new SoCone;

  axis->addChild(finalTranslation);
  axis->addChild(scale);
  axis->addChild(myRotation);
  axis->addChild(rot);
  axis->addChild(zTranslation);
  if ( conicalShaft.getValue() ){
    SoSeparator* shaft = new SoSeparator();
    axis->addChild(shaft);
    SoRotationXYZ* rot = new SoRotationXYZ;
    shaft->addChild(rot);
    SoCone* axisCone = new SoCone;
    shaft->addChild( axisCone );
  }
  else {
    SoCylinder* axisCyl = new SoCylinder;
    axis->addChild( axisCyl );
  }
  
  axis->addChild(cTranslation);
  axis->addChild(axisCone);
  children->append(axis);
}

// generateAlternateRep
void SoArrow::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoArrow::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

