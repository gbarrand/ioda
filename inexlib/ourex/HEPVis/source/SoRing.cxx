// this :
#include <HEPVis/nodes/SoRing.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <HEPVis/SbMath.h>

SbVec3f SoRing::normal(0,0,1);
int SoRing::NbTrianglesPerRing = 60;

SO_NODE_SOURCE(SoRing)

void SoRing::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoRing, SoShape, "Shape");
}

SoRing::SoRing()
{
  SO_NODE_CONSTRUCTOR(SoRing);

  SO_NODE_ADD_FIELD(center, (0,0) );
  SO_NODE_ADD_FIELD(innerRadius, (0.2F));
  SO_NODE_ADD_FIELD(outerRadius, (1));
  SO_NODE_ADD_FIELD(sweepAngle, (360));
}


void SoRing::generatePrimitives(SoAction* action)
{
  SoPrimitiveVertex pv;

  int nbSlices = (int)((float)NbTrianglesPerRing*sweepAngle.getValue()/360.F);
  if ( nbSlices < 3 ) nbSlices = 3;
  float angleInc = (sweepAngle.getValue())*FM_PI/180.F/float(nbSlices);

  float R1 = innerRadius.getValue();
  float R2 = outerRadius.getValue();
  float radiiRatio = R1/R2;
  float Xc, Yc;
  center.getValue().getValue(Xc,Yc);

  beginShape(action, SoShape::TRIANGLE_STRIP);

  float angle = sweepAngle.getValue()*FM_PI/180.F;

  // point at outer radius
  pv.setPoint(SbVec3f(R2*FCOS(angle)+Xc,R2*FSIN(angle)+Yc,0));
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 
                              0.5f*(1+FSIN(angle)), 0, 1));
  shapeVertex(&pv);

  // point at inner radius
  pv.setPoint(SbVec3f(R1*FCOS(angle)+Xc,R1*FSIN(angle)+Yc,0));
  pv.setNormal(normal);
  pv.setTextureCoords(SbVec4f(0.5f*(1+radiiRatio*FCOS(angle)), 
                              0.5f*(1+radiiRatio*FSIN(angle)), 0, 1));
  shapeVertex(&pv);


  for ( int i = 0; i < nbSlices; i++ )
  {
    angle -= angleInc;

    // outer radius
    pv.setPoint(SbVec3f((float)(R2*FCOS(angle))+Xc,(float)(R2*FSIN(angle))+Yc,0));
    pv.setNormal(normal);
    pv.setTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 
                                0.5f*(1+FSIN(angle)), 0, 1));
    shapeVertex(&pv);

    // inner radius
    pv.setPoint(SbVec3f((float)(R1*FCOS(angle))+Xc,(float)(R1*FSIN(angle)+Yc),0));
    pv.setNormal(normal);
    pv.setTextureCoords(SbVec4f(0.5f*(1+radiiRatio*FCOS(angle)), 
                                0.5f*(1+radiiRatio*FSIN(angle)), 0, 1));
    shapeVertex(&pv);
  }
  endShape();
}

void SoRing::computeBBox(SoAction*, SbBox3f& box, SbVec3f& BBcenter)
{
  float Xc, Yc;
  center.getValue().getValue(Xc,Yc);

  // centered at origin
  BBcenter.setValue(Xc, Yc, 0.);

  // bounding box
  float R = outerRadius.getValue();
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

void SoRing::rayPick(SoRayPickAction * action)
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
    if ( sqrt(x*x+y*y+z*z) > outerRadius.getValue() ) return;
    if ( sqrt(x*x+y*y+z*z) < innerRadius.getValue() ) return;

    // within angular section ?
    float theta = sweepAngle.getValue();
    float angle = FATAN2(y,x);
    if ( angle < 0. ) angle += 2*FM_PI;
    if ( theta != 360 && 
         angle*180.F/FM_PI > theta ) return;

    if ( action->isBetweenPlanes(intersection) &&
         (pp = action->addIntersection(intersection)) != NULL )
    {
      pp->setObjectNormal(normal);
      pp->setObjectTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 
                                         0.5f*(1+FSIN(angle)), 0, 1));
    }
  }

}
