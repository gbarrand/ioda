// this :
#include <HEPVis/nodes/SoQuad.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>

SbVec3f SoQuad::normal(0.f,0.f,1.f);


SO_NODE_SOURCE(SoQuad)

void SoQuad::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoQuad, SoShape, "Shape");
}

SoQuad::SoQuad()
{
  SO_NODE_CONSTRUCTOR(SoQuad);

  SO_NODE_ADD_FIELD(LLCornerOffset, (-0.5f, -0.5f));
  SO_NODE_ADD_FIELD(LRCornerOffset, (0.5f, -0.5f));
  SO_NODE_ADD_FIELD(URCornerOffset, (0.5f, 0.5f));
  SO_NODE_ADD_FIELD(ULCornerOffset, (-0.5f, 0.5f));
}


void SoQuad::generatePrimitives(SoAction* action)
{
  // Will generate a single quad

  SoPrimitiveVertex pv;

  // half base & height as SbVec3f
  float x,y;
  SbVec2f params;
  params = LLCornerOffset.getValue();
  params.getValue(x,y);
  SbVec3f LL(x, y, 0.f);

  params = LRCornerOffset.getValue();
  params.getValue(x,y);
  SbVec3f LR(x, y, 0.f);
  params = URCornerOffset.getValue();
  params.getValue(x,y);
  SbVec3f UR(x, y, 0.f);

  params = ULCornerOffset.getValue();
  params.getValue(x,y);
  SbVec3f UL(x, y, 0.f);

  beginShape(action, SoShape::POLYGON);

  // lower left
  pv.setPoint(LL);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.f, 0.f, 0.f, 1.f));
  shapeVertex(&pv);

  // lower right
  pv.setPoint(LR);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(1.f, 0.f, 0.f, 1.f));
  shapeVertex(&pv);

  // upper right
  pv.setPoint(UR);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(1.f, 1.f, 0.f, 1.f));
  shapeVertex(&pv);

  // upper left
  pv.setPoint(UL);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.f, 1.f, 0.f, 1.f));
  shapeVertex(&pv);

  endShape();
}

void SoQuad::computeBBox(SoAction*, SbBox3f& box, SbVec3f& center)
{
  // get min and max on x and y
  float Xmin = 1.e20f;
  float Xmax = -1.e20f;
  float Ymin = 1.e20f;
  float Ymax = -1.e20f;
  float x,y;
  SbVec2f params;

#define SET_MIN_MAX(corner) \
  params = corner.getValue(); \
  params.getValue(x,y); \
  if ( x < Xmin ) Xmin = x; \
  if ( x > Xmax ) Xmax = x; \
  if ( y < Ymin ) Ymin = y; \
  if ( y > Ymax ) Ymax = y; 

  SET_MIN_MAX(LLCornerOffset);
  SET_MIN_MAX(LRCornerOffset);
  SET_MIN_MAX(URCornerOffset);
  SET_MIN_MAX(ULCornerOffset);

  // centered at center of bounding box
  center.setValue(0.5f*(Xmin+Xmax), 0.5f*(Ymin+Ymax), 0.f);

  // bounding box
  SbVec3f corner1(Xmin,Ymin,0.f);
  SbVec3f corner2(Xmax,Ymax,0.f);
  box.setBounds(corner1,corner2);
}
