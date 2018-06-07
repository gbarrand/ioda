/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoHelicalTrack                                          */
/* Description:      Represents a helical trajectory with axis along z       */
/* Author:           Joe Boudreau Nov 7 1996                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <math.h>
#include <assert.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoHelicalTrack.h>
// This statement is required
SO_NODE_SOURCE(SoHelicalTrack)

// Constructor
SoHelicalTrack::SoHelicalTrack() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoHelicalTrack);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(inverseRadius,       (0));
  SO_NODE_ADD_FIELD(cotTheta,            (0));
  SO_NODE_ADD_FIELD(phi0,                (0));
  SO_NODE_ADD_FIELD(d0,                  (0));
  SO_NODE_ADD_FIELD(z0,                  (0));
  SO_NODE_ADD_FIELD(s0,                  (0));
  SO_NODE_ADD_FIELD(s1,                  (1));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  children = new SoChildList(this);
}

// Destructor
SoHelicalTrack::~SoHelicalTrack() {
}


// initClass
void SoHelicalTrack::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoHelicalTrack,SoShape,"Shape");
}

//-------------------------------
// generatePrimitives
// precision of sixteen micron:
#define PRECISION 16.00E-4
//-------------------------------
void SoHelicalTrack::generatePrimitives(SoAction *action) {
  

  SoPrimitiveVertex pv0,pv1;

  //
  // Generate the number of points:  2 minimum, 180+2 for 360 degrees.
  //
  double myS1=s1.getValue();
  double myS0=s0.getValue();
  double w=inverseRadius.getValue();
  float SinTheta    = (float)sin(M_PI/2 - atan(cotTheta.getValue()));
  // fixed angle:
  //int HTMPL=2+180*fabs((myS1-myS0)*inverseRadius.getValue()*SinTheta/(2*M_PI));
  // fixed precision (1 micron):
  int HTMPL = (int)(2+ fabs((myS1-myS0)*SinTheta)*sqrt(fabs(w/(8*PRECISION))));
  double delta = (myS1-myS0)/HTMPL;
  
#ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
  beginShape(action,POLYGON);
  endShape();
#endif

  SbVec3f   OldPosition;
  getTrackPosition(0,(float)myS0,(float)delta,OldPosition);
  SbVec3f   Normal(0,0,0);
  SbVec4f   TextureCoords(0,0,0,0);
  SoDetail  *Detail=NULL;
  for (int i=1;i<HTMPL;i++) {
    SbVec3f TrackPosition; 
    getTrackPosition(i,(float)myS0,(float)delta,TrackPosition);
    pv0.setPoint(OldPosition);
    pv0.setNormal(Normal);
    pv0.setTextureCoords(TextureCoords);
    pv0.setDetail(Detail);

    pv1.setPoint(TrackPosition);
    pv1.setNormal(Normal);
    pv1.setTextureCoords(TextureCoords);
    pv1.setDetail(Detail);
    invokeLineSegmentCallbacks(action,&pv0,&pv1);
    OldPosition=TrackPosition;
  }
}

// getChildren
SoChildList *SoHelicalTrack::getChildren() const {
  return children;
}


// computeBBox
void SoHelicalTrack::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center) {
  float WHOPPING=100000000;
  SbVec3f vmin,vmax;
  SbVec3f A(0,0,0),B(0,0,0),B0(WHOPPING,WHOPPING,WHOPPING),B1(-WHOPPING,-WHOPPING,-WHOPPING);
  double myS1=s1.getValue();
  double myS0=s0.getValue();
  double delta=(myS1-myS0)/13;
  for (int i=0;i<13;i++) {
    getTrackPosition(i,(float)myS0,(float)delta,A);
    B+=A;
    for (int j=0;j<3;j++) {
      if (A[j]<B0[j]) B0[j]=A[j];
      if (A[j]<B0[j]) B0[j]=A[j];
      if (A[j]>B1[j]) B1[j]=A[j];
    }
    B= B/13;
  }
    
  center.setValue(B[0],B[1],B[2]);
  vmin.setValue(B0[0],B0[1],B0[2]);
  vmax.setValue(B1[0],B1[1],B1[2]);
  box.setBounds(vmin,vmax);
}


// updateChildren
void SoHelicalTrack::updateChildren() {
    
  float SinTheta    = (float)sin(M_PI/2 - atan(cotTheta.getValue()));
  int HTMPL=(int)(2+180*fabs((s1.getValue()-s0.getValue())*inverseRadius.getValue()*SinTheta/(2*M_PI)));
  
  // This routine simply draws the points along the helix.  It's invoked
  // whenever primitives are generated or a render action occurs.  
  
  assert(children->getLength()==1);
  SoSeparator   *sep           = (SoSeparator *)  ( *children)[0];
  SoCoordinate3 *controlPts    = (SoCoordinate3 *)( sep->getChild(0));
  controlPts->point.deleteValues(0);
  double myS0=s0.getValue();
  double myS1=s1.getValue();
  double delta = (myS1-myS0)/HTMPL;
  for (int i=0;i<HTMPL;i++) {
    SbVec3f TrackPosition;
    getTrackPosition(i,(float)myS0,(float)delta,TrackPosition);
    controlPts->point.set1Value(i,TrackPosition);
  }
}

// generateChildren
void SoHelicalTrack::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine. 

  assert(children->getLength() ==0);
  SoSeparator   *sep = new SoSeparator(); 
  SoCoordinate3 *controlPts = new SoCoordinate3;
  SoLineSet     *curve      = new SoLineSet;
  sep->addChild(controlPts);
  sep->addChild(curve);
  children->append(sep);
}

// generateAlternateRep
void SoHelicalTrack::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoHelicalTrack::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

// getTrackPosition
void SoHelicalTrack::getTrackPosition(int i, float as0, float deltaS, SbVec3f & pos){

  double s = as0+i*deltaS;

  // get the trajectory as a function of distance
  // this routine is for the track class.  Another class might
  // substitute it's own helper routines here.

  if (i==0) {
    float Phi0     = phi0.getValue();
    float CotTheta = cotTheta.getValue();
    float Theta    = (float)(M_PI/2 - atan(CotTheta));
    _cosTheta = cos(Theta);
    _sinTheta = sin(Theta);
    _cosPhi0=cos(Phi0);
    _sinPhi0=sin(Phi0);
    _w      = inverseRadius.getValue();
    _D0     = d0.getValue();
    _Z0     = z0.getValue();

    double Phi1     = as0*_w*_sinTheta;
    double Delta    = deltaS*_w*_sinTheta;
    _cosPhi1=cos(Phi1);
    _sinPhi1=sin(Phi1);
    _sinDeltaPhi = sin(Delta);
    _cosDeltaPhi = cos(Delta);
  }
  else {
    inc(_sinPhi1,_cosPhi1,_sinDeltaPhi,_cosDeltaPhi);
  }
  if (s==0 || _w==0) {
    pos.setValue( 
 (float)(_D0*_sinPhi0 + s*_sinTheta*(_cosPhi0*_cosPhi1-_sinPhi0*_sinPhi1)),
 (float)(-_D0*_cosPhi0 + s*_sinTheta*(_cosPhi0*_sinPhi1+_sinPhi0*_cosPhi1)),
 (float)(_Z0 + s*_cosTheta));
  }
  else {
    pos.setValue (
  (float)((_cosPhi0*_sinPhi1-_sinPhi0*(-_w*_D0+1-_cosPhi1))/_w),       
  (float)((_sinPhi0*_sinPhi1+_cosPhi0*(-_w*_D0+1-_cosPhi1))/_w),       
  (float)(s*_cosTheta + _Z0));
  }
}
