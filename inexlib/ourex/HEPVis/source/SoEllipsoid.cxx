/*-----------------------------HEPVis---------------------------------------*/
/*                                                                          */
/* Node:             SoEllipsoid                                            */
/* Description:      Represents an Ellipsoid                                */
/* Author:           Joe Boudreau Nov 11 1996                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

// this :
#include <HEPVis/nodes/SoEllipsoid.h>

#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>

// This statement is required
SO_NODE_SOURCE(SoEllipsoid)

// initClass
void SoEllipsoid::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoEllipsoid,SoShape,"Shape");
}

// Constructor
SoEllipsoid::SoEllipsoid() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoEllipsoid);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(eigenvalues,(1.0,1.0,1.0));
  SO_NODE_ADD_FIELD(rotation,(SbVec3f(0,0,1),0));
  SO_NODE_ADD_FIELD(center,(SbVec3f(0,0,0)));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  children = new SoChildList(this);
}

// Destructor
SoEllipsoid::~SoEllipsoid() {
  delete children;
}


// GLRrender
void SoEllipsoid::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoEllipsoid::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoEllipsoid::getChildren() const {
  return children;
}


// computeBBox
void SoEllipsoid::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
  float Extent = eigenvalues.getValue().length();
  SbVec3f vmin(-Extent,-Extent,-Extent), 
          vmax( Extent, Extent, Extent);
  vmin = vmin+center.getValue();
  vmax = vmax+center.getValue();
  TheCenter=center.getValue();
  box.setBounds(vmin,vmax);
}




// updateChildren
void SoEllipsoid::updateChildren() {

}

// generateChildren
void SoEllipsoid::generateChildren() {

  // A box consists of a set of scale factors and a 
  // cube.

  assert(children->getLength() ==0);
  SoSeparator      *lod              = new SoSeparator(); 
  SoSeparator      *sep              = new SoSeparator();
  SoTranslation    *trans            = new SoTranslation();
  SoRotation       *rot              = new SoRotation();
  SoScale          *scale            = new SoScale();
  SoSphere         *sphere           = new SoSphere();

  //  lod->screenArea=100;
  rot->rotation.connectFrom(&rotation);
  trans->translation.connectFrom(&center);
  scale->scaleFactor.connectFrom(&eigenvalues);

  lod->addChild(sep);
  lod->addChild(new SoSeparator());
  sep->addChild(trans);
  sep->addChild(rot);
  sep->addChild(scale);
  sep->addChild(sphere);
  children->append(lod);
}

// generateAlternateRep
void SoEllipsoid::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoEllipsoid::clearAlternateRep() {
  alternateRep.setValue(NULL);
}



