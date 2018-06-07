/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoVtxReconTrack                                          */
/* Description:      Represents a Recon trajectory with axis along z       */
/* Author:           Joe Boudreau Nov 7 1996                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoEllipticalPrism.h>
#include <HEPVis/nodes/SoEllipsoid.h>
#include <HEPVis/nodes/SoVtxReconTrack.h>

// This statement is required
SO_NODE_SOURCE(SoVtxReconTrack)

// initClass
void SoVtxReconTrack::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoVtxReconTrack,SoShape,"Shape");
}

// Constructor
SoVtxReconTrack::SoVtxReconTrack() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoVtxReconTrack);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(cotTheta,            (0));
  SO_NODE_ADD_FIELD(phi0,                (0));
  SO_NODE_ADD_FIELD(ellipsoid,           (NULL));
  SO_NODE_ADD_FIELD(s0,                  (0));
  SO_NODE_ADD_FIELD(s1,                  (1));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  children = new SoChildList(this);
}

// Destructor
SoVtxReconTrack::~SoVtxReconTrack() {
  delete children;
}

// GLRrender
void SoVtxReconTrack::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoVtxReconTrack::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoVtxReconTrack::getChildren() const {
  return children;
}


// computeBBox
void SoVtxReconTrack::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center) {
  SbVec3f vmin,vmax;
  const float IMMENSE=1.0E8;
  SbVec3f A(0,0,0),
    B(0,0,0),
    B0(IMMENSE,IMMENSE,IMMENSE),
    B1(-IMMENSE,-IMMENSE,-IMMENSE);
  for (int i=0;i<2;i++) {
    if (i==0) A=getTrackPosition(s0.getValue());
    if (i==1) A=getTrackPosition(s1.getValue());
    B+=A;
    for (int j=0;j<3;j++) {
      if (A[j]<B0[j]) B0[j]=A[j];
      if (A[j]>B1[j]) B1[j]=A[j];
    }
    B= B/2;
  }
  center.setValue(B[0],B[1],B[2]);
  vmin.setValue(B0[0],B0[1],B0[2]);
  vmax.setValue(B1[0],B1[1],B1[2]);
  vmin = vmin - SbVec3f(1,1,1);
  vmax = vmax + SbVec3f(1,1,1);
  box.setBounds(vmin,vmax);
}


// updateChildren
void SoVtxReconTrack::updateChildren() {


  // This routine simply draws the points along the helix.  It's invoked
  // whenever primitives are generated or a render action occurs.  

  assert(children->getLength()==1);
  // Make sure there's an ellipsoid:
  SoNode *ANode = ellipsoid.getValue();
  if (!ANode) return;
  if (!ANode->isOfType(SoEllipsoid::getClassTypeId())) return;
  SoEllipsoid* TheEllipsoid = (SoEllipsoid*)ANode;
 

  SoSeparator       *sep           = (SoSeparator *)       ( *children)[0];
  SoTranslation     *trans         = (SoTranslation *)     sep->getChild(0);
  SoRotation        *moveToTheta   = (SoRotation *)        sep->getChild(1);
  SoRotation        *moveToPhi     = (SoRotation *)        sep->getChild(2);
  SoEllipticalPrism *ePrism        = (SoEllipticalPrism *) sep->getChild(4);

  float sig0prime = 0, sig1prime=0, angle=0;

  // Get some important axes: u,v,w
  SbVec3f w=getTrackDirection(); 
  w.normalize();
  SbVec3f v(-FSIN(phi0.getValue()),FCOS(phi0.getValue()),0);
  SbVec3f u = v.cross(w); 
 u.normalize();
  SbVec3f uprime,vprime;
  SbRotation InverseRotation = TheEllipsoid->rotation.getValue().inverse();
  InverseRotation.multVec(v,vprime);
  InverseRotation.multVec(u,uprime);
  float sig0,sig1,corr;
  SbVec3f eigenvalues = TheEllipsoid->eigenvalues.getValue();
  sig0 = FSQRT(eigenvalues[0]*eigenvalues[0]*uprime[0]*uprime[0] +
	       eigenvalues[1]*eigenvalues[1]*uprime[1]*uprime[1] +
	       eigenvalues[2]*eigenvalues[2]*uprime[2]*uprime[2]);
  sig1 = FSQRT(eigenvalues[0]*eigenvalues[0]*vprime[0]*vprime[0] +
	       eigenvalues[1]*eigenvalues[1]*vprime[1]*vprime[1] +
	       eigenvalues[2]*eigenvalues[2]*vprime[2]*vprime[2]);
  corr =     (eigenvalues[0]*eigenvalues[0]*uprime[0]*vprime[0] +
              eigenvalues[1]*eigenvalues[1]*uprime[1]*vprime[1] +
              eigenvalues[2]*eigenvalues[2]*uprime[2]*vprime[2])/(sig0*sig1);
  
  getAngle(sig0,sig1,corr, angle,sig0prime,sig1prime);
  ePrism->rotation.setValue(SbVec3f(0,0,1),angle);
  ePrism->eigenvalues.setValue(sig0prime,sig1prime);
  moveToPhi->rotation.setValue(SbVec3f(0,0,1),phi0.getValue());
  moveToTheta->rotation.setValue(
	  SbVec3f(-FSIN(phi0.getValue()),FCOS(phi0.getValue()),0),
	  -FATAN(cotTheta.getValue()));
  trans->translation.setValue(getTrackPosition(0)); 
}

// generateChildren
void SoVtxReconTrack::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine. 

  assert(children->getLength()==0);

  // We're going to need a quick calculation here:
  SoCalculator  *theCalculator = new SoCalculator();
  theCalculator->a.connectFrom(&s1);
  theCalculator->b.connectFrom(&s0);
  theCalculator->expression.set1Value(0,"oA=vec3f(0,0,(a-b)/2 + b)");
  theCalculator->expression.set1Value(1,"oa=(a-b)/2");

  SoSeparator       *sep = new SoSeparator(); 
  SoTranslation     *trans = new SoTranslation();
  SoRotation        *moveToTheta = new SoRotation();
  SoRotation        *moveToPhi   = new SoRotation();
  SoRotation        *flipAroundY = new SoRotation();    
  SoEllipticalPrism *ePrism = new SoEllipticalPrism();

  flipAroundY->rotation.setValue(SbVec3f(0,1,0),(float)M_PI/2);
 
  ePrism->extent.connectFrom(&theCalculator->oa);
  ePrism->center.connectFrom(&theCalculator->oA);
  
  sep->addChild(trans);
  sep->addChild(moveToTheta);
  sep->addChild(moveToPhi);
  sep->addChild(flipAroundY);
  sep->addChild(ePrism);

  children->append(sep);
}

// generateAlternateRep
void SoVtxReconTrack::generateAlternateRep(SoAction* aAction) {

  // This routine sets the alternate representation to the child
  // list of this mode.  


  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((*children)[0]);
  SoSeparator *TheSeparator = (SoSeparator *) alternateRep.getValue();
  if (!TheSeparator) return;
  SoEllipticalPrism *ePrism = (SoEllipticalPrism *) TheSeparator->getChild(4);
  ePrism->generateAlternateRep(aAction);

}

// clearAlternateRep
void SoVtxReconTrack::clearAlternateRep() {
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((*children)[0]);
  SoSeparator *TheSeparator = (SoSeparator *) alternateRep.getValue();
  if (!TheSeparator) return;
  SoEllipticalPrism *ePrism = (SoEllipticalPrism *) TheSeparator->getChild(4);
  ePrism->clearAlternateRep();

  alternateRep.setValue(NULL);
}

// getTrackPosition
SbVec3f SoVtxReconTrack::getTrackPosition(float s){

  // get the trajectory as a function of distance
  // this routine is for the track class.  Another class might
  // substitute it's own helper routines here.

  SoNode *ANode = ellipsoid.getValue();
  if (!ANode) return SbVec3f(0,0,0);
  if (!ANode->isOfType(SoEllipsoid::getClassTypeId())) return SbVec3f(0,0,0);
  SoEllipsoid *TheEllipsoid = (SoEllipsoid *) ANode;

  float CotTheta = cotTheta.getValue();
  float Phi0     = phi0.getValue();
  float Theta    = (float)M_PI/2 - FATAN(CotTheta);
  float CosTheta = FCOS(Theta);
  float SinTheta = FSIN(Theta);

  float phi1     = Phi0;
  float xtan     = SinTheta*FCOS(phi1);
  float ytan     = SinTheta*FSIN(phi1);
  float ztan     = CosTheta;

  return SbVec3f(TheEllipsoid->center.getValue()+SbVec3f(xtan,ytan,ztan)*s);
}




// getTrackPosition
SbVec3f SoVtxReconTrack::getTrackDirection(){

  // get the trajectory as a function of distance
  // this routine is for the track class.  Another class might
  // substitute it's own helper routines here.

  float CotTheta = cotTheta.getValue();
  float Phi0     = phi0.getValue();
  float Theta    = (float)M_PI/2 - FATAN(CotTheta);
  float CosTheta = FCOS(Theta);
  float SinTheta = FSIN(Theta);

  float phi1     = Phi0;
  float xtan     = SinTheta*FCOS(phi1);
  float ytan     = SinTheta*FSIN(phi1);
  float ztan     = CosTheta;

  return SbVec3f(xtan,ytan,ztan);
}




void SoVtxReconTrack::getAngle(float sig0,   float sig1,       float corr,
                               float &angle, float &sig0prime, float &sig1prime) {


  if (sig0==sig1 || corr == 0){ 
    sig0prime=sig0;
    sig1prime=sig1;
    angle = 0;
  }
  else {
    double tmp= -4*(1-corr*corr)*sig0*sig0*sig1*sig1 + 
                 pow((sig0*sig0+sig1*sig1),2);
    if (tmp<0) tmp=0;

    double flipSqrt=1;
    if (sig0*sig0>sig1*sig1) flipSqrt=-1;

    //    angle = 0.5F * atan2(2*(corr*sig0*sig1),(sig1*sig1-sig0*sig0));
    angle = FATAN( (1/(2*corr*sig0*sig1)) * 
		  (sig1*sig1 - sig0*sig0 + flipSqrt*FSQRT(tmp)));
    sig0prime = 
      (float)(0.5F * ((sig0*sig0 + sig1*sig1) + flipSqrt*FSQRT(tmp)));
    sig1prime = 
      (float)(0.5F * ((sig0*sig0 + sig1*sig1) - flipSqrt*FSQRT(tmp)));
    assert (sig0prime>=0);
    assert (sig1prime>=0);
    sig0prime = FSQRT(sig0prime);
    sig1prime = FSQRT(sig1prime);
  }
}





