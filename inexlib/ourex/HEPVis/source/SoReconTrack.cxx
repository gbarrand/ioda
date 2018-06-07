/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoReconTrack                                            */
/* Description:      Represents a Recon trajectory with axis along z         */
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
#include <HEPVis/nodes/SoReconTrack.h>

// This statement is required
SO_NODE_SOURCE(SoReconTrack)

// initClass
void SoReconTrack::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoReconTrack,SoShape,"Shape");
}


// Constructor
SoReconTrack::SoReconTrack() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoReconTrack);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(cotTheta,            (0.0));
  SO_NODE_ADD_FIELD(phi0,                (0.0));
  SO_NODE_ADD_FIELD(d0,                  (0.0));
  SO_NODE_ADD_FIELD(z0,                  (0.0));
  SO_NODE_ADD_FIELD(sigD0,               (1.0));
  SO_NODE_ADD_FIELD(sigZ0,               (1.0));
  SO_NODE_ADD_FIELD(correlation,         (0.0));
  SO_NODE_ADD_FIELD(s0,                  (0.0));
  SO_NODE_ADD_FIELD(s1,                  (1.0));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  children = new SoChildList(this);
}

// Destructor
SoReconTrack::~SoReconTrack() {
  delete children;
}


// GLRrender
void SoReconTrack::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoReconTrack::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList *SoReconTrack::getChildren() const {
  return children;
}


// computeBBox
void SoReconTrack::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center) {
  SbVec3f vmin,vmax;
  const float IMMENSE=1000000000;
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
    B= B/2.0F;
  }
  center.setValue(B[0],B[1],B[2]);
  vmin.setValue(B0[0],B0[1],B0[2]);
  vmax.setValue(B1[0],B1[1],B1[2]);
  vmin = vmin - 
   SbVec3f(1,1,1) * FSQRT(FPOW(sigD0.getValue(),2) + FPOW(sigZ0.getValue(),2));
  vmax = vmax + 
   SbVec3f(1,1,1) * FSQRT(FPOW(sigD0.getValue(),2) + FPOW(sigZ0.getValue(),2));
  box.setBounds(vmin,vmax);
}


// updateChildren
void SoReconTrack::updateChildren() {


  // This routine simply draws the points along the helix.  It's invoked
  // whenever primitives are generated or a render action occurs.  

  assert(children->getLength()==1);
  SoSeparator       *sep           = (SoSeparator *)       ( *children)[0];
  SoTranslation     *trans         = (SoTranslation *)     sep->getChild(0);
  SoRotation        *moveToTheta   = (SoRotation *)        sep->getChild(1);
  SoRotation        *moveToPhi     = (SoRotation *)        sep->getChild(2);
  SoEllipticalPrism *ePrism        = (SoEllipticalPrism *) sep->getChild(5);
  float angle,sig0prime,sig1prime;
  getAngle(angle, sig0prime, sig1prime);
  ePrism->rotation.setValue(SbVec3f(0,0,1),angle);
  ePrism->eigenvalues.setValue(sig0prime,sig1prime);
  moveToPhi->rotation.setValue(SbVec3f(0,0,1),phi0.getValue());
  moveToTheta->rotation.setValue(SbVec3f(-FSIN(phi0.getValue()),
					 FCOS(phi0.getValue()),
					 0),
				 -FATAN(cotTheta.getValue()));
  trans->translation.setValue(getTrackPosition(0)); 
}

// generateChildren
void SoReconTrack::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine. 

  assert(children->getLength() ==0);

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
  SoRotation        *flipAroundZ = new SoRotation();
  SoEllipticalPrism *ePrism = new SoEllipticalPrism();

  flipAroundY->rotation.setValue(SbVec3f(0,1,0),(float)M_PI/2);
  flipAroundZ->rotation.setValue(SbVec3f(0,0,1),(float)M_PI/2);  
 
  ePrism->extent.connectFrom(&theCalculator->oa);
  ePrism->center.connectFrom(&theCalculator->oA);
  
  sep->addChild(trans);
  sep->addChild(moveToTheta);
  sep->addChild(moveToPhi);
  sep->addChild(flipAroundY);
  sep->addChild(flipAroundZ);
  sep->addChild(ePrism);
  children->append(sep);
}

// generateAlternateRep
void SoReconTrack::generateAlternateRep(SoAction* aAction) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue(( *children)[0]);
  SoSeparator* TheSeparator = (SoSeparator*)alternateRep.getValue();
  if (!TheSeparator) return;
  SoEllipticalPrism* ePrism = (SoEllipticalPrism*)TheSeparator->getChild(5);
  ePrism->generateAlternateRep(aAction);
}

// clearAlternateRep
void SoReconTrack::clearAlternateRep() {
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue(( *children)[0]);
  SoSeparator *TheSeparator = (SoSeparator *) alternateRep.getValue();
  if (!TheSeparator) return;
  SoEllipticalPrism* ePrism = (SoEllipticalPrism*)TheSeparator->getChild(5);
  ePrism->clearAlternateRep();

  alternateRep.setValue(NULL);
}

// getTrackPosition
SbVec3f SoReconTrack::getTrackPosition(float s){

  // get the trajectory as a function of distance
  // this routine is for the track class.  Another class might
  // substitute it's own helper routines here.

  float CotTheta = cotTheta.getValue();
  float Phi0     = phi0.getValue();
  float Del0       = d0.getValue();
  float Z0       = z0.getValue();
  float Theta    = (float)(M_PI/2 - FATAN(CotTheta));
  float CosTheta = FCOS(Theta);
  float SinTheta = FSIN(Theta);

  float phi1     = Phi0;
  float xtan     = SinTheta * FCOS(phi1);
  float ytan     = SinTheta * FSIN(phi1);
  float ztan     = CosTheta;
  return SbVec3f(-Del0 * FCOS(Phi0+M_PI/2),
		 -Del0 * FSIN(Phi0+M_PI/2),
		 Z0) + SbVec3f(xtan,ytan,ztan)*s;
}


void SoReconTrack::getAngle(float &angle, float &sig0prime, float &sig1prime) {

  float sig0     = sigD0.getValue();
  float sig1     = sigZ0.getValue();
  float corr     = correlation.getValue();
  float CotTheta = cotTheta.getValue();
  float geomFactor = FSQRT(1+CotTheta*CotTheta);
  sig1 /= geomFactor;

  if (sig0==sig1 || corr == 0.0){ 
    sig0prime=sig0;
    sig1prime=sig1;
    angle = 0;
  }
  else {

    float tmp = 
      -4*(1-corr*corr)*sig0*sig0*sig1*sig1 + FPOW((sig0*sig0+sig1*sig1),2);
    if (tmp<0) tmp=0;

    float flipSqrt=1;
    if (sig0*sig0>sig1*sig1) flipSqrt=-1;

    angle = FATAN( (1/(2.0*corr*sig0*sig1)) * (sig1*sig1 - sig0*sig0 + 
					      flipSqrt * FSQRT(tmp)));
    sig0prime = 0.5F * ((sig0*sig0 + sig1*sig1) + flipSqrt * FSQRT(tmp));
    sig1prime = 0.5F * ((sig0*sig0 + sig1*sig1) - flipSqrt * FSQRT(tmp));
    assert (sig0prime>=0);
    assert (sig1prime>=0);
    sig0prime = FSQRT(sig0prime);
    sig1prime = FSQRT(sig1prime);
  }
}


