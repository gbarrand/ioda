// this :
#include <HEPVis/nodes/SoTriangle.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>

SbVec3f SoTriangle::normal(0.f,0.f,1.f);


SO_NODE_SOURCE(SoTriangle)

void SoTriangle::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoTriangle, SoShape, "Shape");
}

SoTriangle::SoTriangle()
{
  SO_NODE_CONSTRUCTOR(SoTriangle);

  SO_NODE_ADD_FIELD(Corner1Offset, (-0.5f, -0.5f));
  SO_NODE_ADD_FIELD(Corner2Offset, (0.5f, 0.f));
  SO_NODE_ADD_FIELD(Corner3Offset, (-0.5f, 0.5f));
}


void SoTriangle::generatePrimitives(SoAction* action)
{
  // Will generate a single triangle

  SoPrimitiveVertex pv;

  // half base & height as SbVec3f
  float x,y;
  SbVec2f params;
  params = Corner1Offset.getValue();
  params.getValue(x,y);
  SbVec3f C1(x, y, 0.);

  params = Corner2Offset.getValue();
  params.getValue(x,y);
  SbVec3f C2(x, y, 0.);

  params = Corner3Offset.getValue();
  params.getValue(x,y);
  SbVec3f C3(x, y, 0.);

  beginShape(action, SoShape::TRIANGLES);

  pv.setPoint(C1);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.f, 0.f, 0.f, 1.f));
  shapeVertex(&pv);

  pv.setPoint(C2);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(1.f, 0.5f, 0.f, 1.f));
  shapeVertex(&pv);

  pv.setPoint(C3);
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.f, 1.f, 0.f, 1.f));
  shapeVertex(&pv);


  endShape();
}

void SoTriangle::computeBBox(SoAction*, SbBox3f& box, SbVec3f& center)
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

  SET_MIN_MAX(Corner1Offset);
  SET_MIN_MAX(Corner2Offset);
  SET_MIN_MAX(Corner3Offset);

  // centered at center of bounding box
  center.setValue(0.5f*(Xmin+Xmax), 0.5f*(Ymin+Ymax), 0.f);

  // bounding box
  SbVec3f corner1(Xmin,Ymin,0.f);
  SbVec3f corner2(Xmax,Ymax,0.f);
  box.setBounds(corner1,corner2);
}
