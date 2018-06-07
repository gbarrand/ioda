// this :
#include <HEPVis/nodes/So2DArrow.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>

SbVec3f So2DArrow::normal(0.f,0.f,1.f);

SO_NODE_SOURCE(So2DArrow)

void So2DArrow::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(So2DArrow, SoShape, "Shape");
}

So2DArrow::So2DArrow()
{
  SO_NODE_CONSTRUCTOR(So2DArrow);

  SO_NODE_ADD_FIELD(length, (1.0f));
  SO_NODE_ADD_FIELD(lengthRatio, (0.1f));
  SO_NODE_ADD_FIELD(headHeight, (0.05f));
}


void So2DArrow::generatePrimitives(SoAction* action)
{
  // Will generate a single quad

  SoPrimitiveVertex pv;

  pv.setNormal(normal);

  float lRatio = lengthRatio.getValue();
  float hh = headHeight.getValue();
  float l = length.getValue();

  SoPrimitiveVertex beginLine, endLine;
  beginLine.setPoint(SbVec3f(0.f,0.f,0.f));
  beginLine.setNormal(normal);
  endLine.setPoint(SbVec3f((1.f-lRatio)*l, 0.f, 0.f));
  endLine.setNormal(normal);
#ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
  beginShape(action,POLYGON);
  endShape();
#endif
  invokeLineSegmentCallbacks(action, &beginLine, &endLine);

  // triangle
  beginShape(action, SoShape::TRIANGLES);
  pv.setPoint(SbVec3f((1.f-lRatio)*l, -0.5f*hh, 0.f));
  pv.setTextureCoords(SbVec4f(0.f, 0.f, 0.f, 1.f));
  shapeVertex(&pv);
  pv.setPoint(SbVec3f(l, 0.f, 0.f));
  pv.setTextureCoords(SbVec4f(1.f, 0.f, 0.f, 1.f));
  shapeVertex(&pv);
  pv.setPoint(SbVec3f((1.f-lRatio)*l, 0.5f*hh, 0.f));
  pv.setTextureCoords(SbVec4f(0.f, 1.f, 0.f, 1.f));
  shapeVertex(&pv);
  endShape();
}

void So2DArrow::computeBBox(SoAction*, SbBox3f& box, SbVec3f& center)
{
  float l = length.getValue();
  float h = headHeight.getValue();

  // center
  center.setValue(0.5f * l, 0.f, 0.f);

  // bounding box
  SbVec3f corner1(0.f, -0.5f*h, 0.f);
  SbVec3f corner2(l, 0.5f*h, 0.f);
  box.setBounds(corner1,corner2);
}
