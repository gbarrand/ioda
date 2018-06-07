// this :
#include <HEPVis/nodes/SoCircleArc.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <HEPVis/SbMath.h>

SbVec3f SoCircleArc::normal(0,0,1);
int SoCircleArc::NbTrianglesPerCircleArc = 60;

SO_NODE_SOURCE(SoCircleArc)

void SoCircleArc::initClass()
{
  // initialize type ID
  SO_NODE_INIT_CLASS(SoCircleArc, SoShape, "Shape");
}

SoCircleArc::SoCircleArc()
{
  SO_NODE_CONSTRUCTOR(SoCircleArc);

  SO_NODE_ADD_FIELD(center, (0,0,0) );
  SO_NODE_ADD_FIELD(radius, (1));
  SO_NODE_ADD_FIELD(sweepAngle, (360));
}


void SoCircleArc::generatePrimitives(SoAction* action)
{
  int nbSlices = (int)(float(NbTrianglesPerCircleArc)*sweepAngle.getValue()/360.f);
  if ( nbSlices < 3 ) nbSlices = 3;
  float angleInc = (sweepAngle.getValue())*FM_PI/180.f/float(nbSlices);
  float R = radius.getValue();
  float Xc, Yc, Zc;
  center.getValue().getValue(Xc,Yc,Zc);

#ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
  beginShape(action,POLYGON);
  endShape();
#endif

  float angle = 0;
  for ( int i = 0; i < nbSlices; i++ )
  {
    SoPrimitiveVertex pv1;
    pv1.setPoint(SbVec3f((float)(R*FCOS(angle))+Xc,(float)(R*FSIN(angle))+Yc,Zc));
    pv1.setNormal(normal);
    pv1.setTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 0.5f*(1+FSIN(angle)), 0, 1));
    angle += angleInc;

    SoPrimitiveVertex pv2;
    pv2.setPoint(SbVec3f((float)(R*FCOS(angle))+Xc,(float)(R*FSIN(angle))+Yc,Zc));
    pv2.setNormal(normal);
    pv2.setTextureCoords(SbVec4f(0.5f*(1+FCOS(angle)), 0.5f*(1+FSIN(angle)), 0, 1));

    invokeLineSegmentCallbacks(action, &pv1, &pv2);
  }

}

void SoCircleArc::computeBBox(SoAction*, SbBox3f& box, SbVec3f& BBcenter)
{
  float Xc, Yc, Zc;
  center.getValue().getValue(Xc,Yc,Zc);

  // centered at origin
  BBcenter.setValue(Xc, Yc, Zc);

  // bounding box
  float R = radius.getValue();
  float theta = sweepAngle.getValue()*FM_PI/180.f;

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

  SbVec3f corner1(Xmin+Xc,Ymin+Yc,Zc);
  SbVec3f corner2(R+Xc,Ymax+Yc,Zc);
  box.setBounds(corner1,corner2);
}

