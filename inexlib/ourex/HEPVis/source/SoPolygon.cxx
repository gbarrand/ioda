// this :
#include <HEPVis/nodes/SoPolygon.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <HEPVis/SbGL.h>

SbVec3f SoPolygon::normal(0.f,0.f,1.f);


SO_NODE_SOURCE(SoPolygon)

void SoPolygon::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoPolygon, SoShape, "Shape");
}

SoPolygon::SoPolygon()
{
  SO_NODE_CONSTRUCTOR(SoPolygon);

  SO_NODE_ADD_FIELD(corners, (0, 0));

  float xy[8][2] = { {0.924f, -0.383f},
                     {0.924f,  0.383f},
                     {0.383f,  0.924f},
                     {-0.383f, 0.924f},
                     {-0.924f, 0.383f},
                     {-0.924f, -0.383f},
                     {-0.383f, -0.924f},
                     {0.383f, -0.924f} };

  corners.setValues(0,8,xy);
}


void SoPolygon::generatePrimitives(SoAction* action)
{
  // Will generate a polygon
 
  int nSeg = corners.getNum();
  if ( nSeg < 2 ) return;

  SoPrimitiveVertex pv;

  const SbVec2f* cornerList = corners.getValues(0);
  float Xmin = 1.e20f;
  float Xmax = -1.e20f;
  float Ymin = 1.e20f;
  float Ymax = -1.e20f;
  for ( int i = 0; i < nSeg; ++i )
  {
    float x,y;
    cornerList[i].getValue(x,y);
    if ( x < Xmin ) Xmin = x;
    if ( x > Xmax ) Xmax = x;
    if ( y < Ymin ) Ymin = y;
    if ( y > Ymax ) Ymax = y;
  }

  beginShape(action, SoShape::POLYGON);
  for ( int i = 0; i < nSeg; ++i )
  {
    float x,y;
    cornerList[i].getValue(x,y);
    pv.setPoint(SbVec3f(x,y,0.));
    pv.setNormal(normal);
    pv.setTextureCoords(SbVec4f((x-Xmin)/(Xmax-Xmin), (y-Ymin)/(Ymax-Ymin), 
                                0.f, 1.f));
    shapeVertex(&pv);
  }
  endShape();
}

void SoPolygon::computeBBox(SoAction*, SbBox3f& box, SbVec3f& center)
{
  int nSeg = corners.getNum();
  if ( nSeg < 2 ) return;

  const SbVec2f* cornerList = corners.getValues(0);
  float Xmin = 1.e20f;
  float Xmax = -1.e20f;
  float Ymin = 1.e20f;
  float Ymax = -1.e20f;
  for ( int i = 0; i < nSeg; ++i )
  {
    float x,y;
    cornerList[i].getValue(x,y);
    if ( x < Xmin ) Xmin = x;
    if ( x > Xmax ) Xmax = x;
    if ( y < Ymin ) Ymin = y;
    if ( y > Ymax ) Ymax = y;
  }


  // centered at center of bounding box
  center.setValue(0.5f*(Xmin+Xmax), 0.5f*(Ymin+Ymax), 0.f);

  // bounding box
  SbVec3f corner1(Xmin,Ymin,0.f);
  SbVec3f corner2(Xmax,Ymax,0.f);
  box.setBounds(corner1,corner2);
}
