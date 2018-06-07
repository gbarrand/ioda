/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoWedge                                                 */
/* Description:      Just a plain ugly wedge                                 */
/* Author:           Peter Hamel                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <HEPVis/nodes/SoWedge.h>

// This statement is required
SO_NODE_SOURCE(SoWedge)

// Constructor
SoWedge::SoWedge() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoWedge);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(thickness,                (0.03F));
  SO_NODE_ADD_FIELD(halfWidth1,               (3));
  SO_NODE_ADD_FIELD(halfWidth2,               (1));
  SO_NODE_ADD_FIELD(halfHeight,                (2));
  SO_NODE_ADD_FIELD(alternateRep,             (NULL));
  
  children = new SoChildList(this);
}

// Destructor
SoWedge::~SoWedge() {
}

// initClass
void SoWedge::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoWedge,SoShape,"Shape");
}

// GLRrender
void SoWedge::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();

  if ((myHalfHeight != halfHeight.getValue()) ||
      (myHalfWidth1 != halfWidth1.getValue()) ||
      (myHalfWidth2 != halfWidth2.getValue()) ||
      (myThickness  != thickness.getValue()))
     updateChildren();
  children->traverse(action);
  myHalfHeight = halfHeight.getValue();
  myHalfWidth1 = halfWidth1.getValue();
  myHalfWidth2 = halfWidth2.getValue();
  myThickness = thickness.getValue();
}

// generatePrimitives
void SoWedge::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoWedge::getChildren() const {
  return children;
}

// computeBBox
void SoWedge::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  float size = halfWidth1.getValue();
  if (halfWidth2.getValue() > size) size = halfWidth2.getValue();
  if (halfHeight.getValue() > size) size = halfHeight.getValue();
  SbVec3f mn(-size,-size,-size), mx(size, size, size);
  center.setValue(0,0,0);
  box.setBounds(mn,mx);
}

// updateChildren
void SoWedge::updateChildren() {

  assert(children->getLength()==1);
  SoSeparator       *sep                = (SoSeparator *)  ( *children)[0];

  sep->removeAllChildren();

  SbVec3f vertexPositions[8] =
   {
    SbVec3f (-.5F*thickness.getValue(), -halfWidth1.getValue(), -halfHeight.getValue()),
    SbVec3f (-.5F*thickness.getValue(), -halfWidth2.getValue(),  halfHeight.getValue()),
    SbVec3f (-.5F*thickness.getValue(),  halfWidth1.getValue(), -halfHeight.getValue()),
    SbVec3f (-.5F*thickness.getValue(),  halfWidth2.getValue(),  halfHeight.getValue()),
    SbVec3f ( .5F*thickness.getValue(), -halfWidth1.getValue(), -halfHeight.getValue()),
    SbVec3f ( .5F*thickness.getValue(), -halfWidth2.getValue(),  halfHeight.getValue()),
    SbVec3f ( .5F*thickness.getValue(),  halfWidth1.getValue(), -halfHeight.getValue()),
    SbVec3f ( .5F*thickness.getValue(),  halfWidth2.getValue(),  halfHeight.getValue())
   };
  int32_t indices[30] =
   {
    0, 1, 3, 2, SO_END_FACE_INDEX,
    4, 6, 7, 5, SO_END_FACE_INDEX,
    0, 4, 5, 1, SO_END_FACE_INDEX,
    6, 2, 3, 7, SO_END_FACE_INDEX,
    0, 2, 6, 4, SO_END_FACE_INDEX,
    1, 5, 7, 3, SO_END_FACE_INDEX
   };
  SoCoordinate3 *theWedgeCoords = new SoCoordinate3;
  theWedgeCoords->point.setValues(0, 8, vertexPositions);
  SoIndexedFaceSet *theWedgeFaceSet = new SoIndexedFaceSet;
  theWedgeFaceSet->coordIndex.setValues(0, 30, indices);
  sep->addChild(theWedgeCoords);
  sep->addChild(theWedgeFaceSet);
}

// generateChildren
void SoWedge::generateChildren() {

  assert(children->getLength() ==0);
  SoSeparator *sep = new SoSeparator();
  children->append(sep);
}

// generateAlternateRep
void SoWedge::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoWedge::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
