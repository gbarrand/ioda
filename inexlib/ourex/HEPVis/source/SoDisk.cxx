// this :
#include <HEPVis/nodes/SoDisk.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <HEPVis/SbMath.h>

SbVec3f SoDisk::normal(0,0,1);
int SoDisk::NbTrianglesPerDisk = 60;

SO_NODE_SOURCE(SoDisk)

void SoDisk::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoDisk, SoShape, "Shape");
}

SoDisk::SoDisk()
{
  SO_NODE_CONSTRUCTOR(SoDisk);

  SO_NODE_ADD_FIELD(center, (0,0) );
  SO_NODE_ADD_FIELD(radius, (1));
  SO_NODE_ADD_FIELD(sweepAngle, (360));
}


void SoDisk::generatePrimitives(SoAction* action)
{
  SoPrimitiveVertex pv;

  int nbSlices = (int)(float(NbTrianglesPerDisk)*sweepAngle.getValue()/360.f);
  if ( nbSlices < 3 ) nbSlices = 3;
  float angleInc = (sweepAngle.getValue())*FM_PI/180.F/float(nbSlices);
  float R = radius.getValue();
  float Xc, Yc;
  center.getValue().getValue(Xc,Yc);

  beginShape(action, SoShape::TRIANGLE_FAN);

  // center point
  pv.setPoint(SbVec3f(Xc,Yc,0));
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.5f, 0.5f, 0, 1));
  shapeVertex(&pv);

  float angle = 0;
  for ( int i = 0; i <= nbSlices; i++ )
  {
    pv.setPoint(SbVec3f((float)(R*FCOS(angle))+Xc,(float)(R*FSIN(angle))+Yc,0));
    pv.setNormal(normal);
    pv.setTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 0.5f*(1+FSIN(angle)), 0, 1));
    shapeVertex(&pv);

    angle += angleInc;
  }

  endShape();
}

void SoDisk::computeBBox(SoAction*, SbBox3f& box, SbVec3f& BBcenter)
{
  float Xc, Yc;
  center.getValue().getValue(Xc,Yc);

  // centered at origin
  BBcenter.setValue(Xc, Yc, 0);

  // bounding box
  float R = radius.getValue();
  float theta = sweepAngle.getValue()*FM_PI/180.F;

  float Xmin, Ymin, Ymax;
  if ( theta < FM_PI_2 ) 
  {
    Ymin = 0;
    Ymax = R*FSIN(theta);
    Xmin = 0;
  }
  else if ( theta < FM_PI )
  {
    Ymin = 0;
    Ymax = R;
    Xmin = R*FCOS(theta);
  }
  else if ( theta < 3*FM_PI_2 )
  {
    Ymin = R*FSIN(theta);
    Ymax = R;
    Xmin = -R;
  }
  else
  {
    Xmin = -R;
    Ymin = -R;
    Ymax = R;
  }

  SbVec3f corner1(Xmin+Xc,Ymin+Yc,0);
  SbVec3f corner2(R+Xc,Ymax+Yc,0);
  box.setBounds(corner1,corner2);
}

void SoDisk::rayPick(SoRayPickAction * action)
{
  // Is it pickable ?
  if ( ! shouldRayPick(action) ) return;

  // compute the picking ray in our current object space
  computeObjectSpaceRay(action);

  SoPickedPoint* pp;
  SbVec3f intersection;
  SbPlane XY(SbVec3f(0,0,1),0);
  if ( XY.intersect(action->getLine(), intersection) )
  {
    float x, y, z;
    intersection.getValue(x, y, z);

    // back to the case of a disk centered at (0,0)
    float Xc, Yc;
    center.getValue().getValue(Xc,Yc);
    x -= Xc;
    y -= Yc;

    // within radius ?
    if ( sqrt(x*x+y*y+z*z) > radius.getValue() ) return;

    // within angular section ?
    float theta = sweepAngle.getValue();
    float angle = FATAN2(y,x);
    if ( angle < 0. ) angle += 2*FM_PI;
    if ( theta != 360 && angle*180.F/FM_PI > theta ) return;

    if ( action->isBetweenPlanes(intersection) &&
         (pp = action->addIntersection(intersection)) != NULL )
    {
      pp->setObjectNormal(normal);
      pp->setObjectTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 
                                         0.5f*(1+FSIN(angle)), 0, 1));
    }
  }

}
