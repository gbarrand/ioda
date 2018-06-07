/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoEllipticalPrism                                       */
/* Description:      Represents an EllipticalPrism                           */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoEllipticalPrism.h>

// This statement is required
SO_NODE_SOURCE(SoEllipticalPrism)

// initClass
void SoEllipticalPrism::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoEllipticalPrism,SoShape,"Shape");
}

// Constructor
SoEllipticalPrism::SoEllipticalPrism() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoEllipticalPrism);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(eigenvalues,              (1.0,1.0));
  SO_NODE_ADD_FIELD(extent,                   (1.0));
  SO_NODE_ADD_FIELD(rotation,                 (SbVec3f(0,0,1),0));
  SO_NODE_ADD_FIELD(center,                   (SbVec3f(0,0,0)));
  SO_NODE_ADD_FIELD(alternateRep,       (NULL));
  children = new SoChildList(this);
}

// Destructor
SoEllipticalPrism::~SoEllipticalPrism() {
  delete children;
}

// GLRrender
void SoEllipticalPrism::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoEllipticalPrism::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoEllipticalPrism::getChildren() const {
  return children;
}


// computeBBox
void SoEllipticalPrism::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
  float Extent = FSQRT(FPOW(eigenvalues.getValue().length(),2) + 
                       FPOW(extent.getValue(),2));
  SbVec3f vmin(-Extent,-Extent,-Extent), 
          vmax( Extent, Extent, Extent);
  vmin = vmin + center.getValue();
  vmax = vmax + center.getValue();
  TheCenter=center.getValue();
  box.setBounds(vmin,vmax);
}




// updateChildren
void SoEllipticalPrism::updateChildren() {

  // ReRender when a value changes.  We don't have to worry about
  // the rotation here, it's field is connected to the basic rotation
  assert(children->getLength()==1);
  SoLevelOfDetail* lod = (SoLevelOfDetail*)(*children)[0];
  SoSeparator* sep = (SoSeparator*)lod->getChild(0);
  SoScale* scale = (SoScale*)sep->getChild(2);
  SbVec2f xy = eigenvalues.getValue();
  float z = extent.getValue();
 
  scale->scaleFactor.setValue(xy[0],xy[1],z);

}

// generateChildren
void SoEllipticalPrism::generateChildren() {

  // A box consists of a set of scale factors and a 
  // cube.

  assert(children->getLength() ==0);
  SoLevelOfDetail  *lod              = new SoLevelOfDetail(); 
  SoSeparator      *sep              = new SoSeparator();
  SoTranslation    *trans            = new SoTranslation();
  SoRotation       *rot              = new SoRotation();
  SoScale          *scale            = new SoScale();
  SoRotation       *over             = new SoRotation();
  SoCylinder       *cyl              = new SoCylinder();

  lod->screenArea.setValue(100);
  over->rotation.setValue(SbVec3f(1,0,0),FM_PI_2);
  trans->translation.connectFrom(&center);
  rot->rotation.connectFrom(&rotation);

  lod->addChild(sep);
  lod->addChild(new SoSeparator());
  sep->addChild(trans);
  sep->addChild(rot);
  sep->addChild(scale);
  sep->addChild(over);
  sep->addChild(cyl);
  children->append(lod);
  updateChildren();
}

// generateAlternateRep
void SoEllipticalPrism::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue(( *children)[0]);
}

// clearAlternateRep
void SoEllipticalPrism::clearAlternateRep() {
  alternateRep.setValue(NULL);
}




