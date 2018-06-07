/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoEllipsoidSegment                                      */
/* Description:      Represents an EllipsoidSegment                          */
/* Author:           Xiaoling Fan                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// this :
#include <HEPVis/nodes/SoEllipsoidSegment.h>

#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoCoordinate3.h>

#include <HEPVis/SbMath.h>

#include <assert.h>

// This statement is required
SO_NODE_SOURCE(SoEllipsoidSegment)

// initClass
void SoEllipsoidSegment::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoEllipsoidSegment,SoShape,"Shape");
}

// Constructor
SoEllipsoidSegment::SoEllipsoidSegment() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoEllipsoidSegment);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(ax,              (1));
  SO_NODE_ADD_FIELD(by,              (1));
  SO_NODE_ADD_FIELD(cz,              (1));
  SO_NODE_ADD_FIELD(fSPhi,           (0));
  SO_NODE_ADD_FIELD(fDPhi,           (2*FM_PI));
  SO_NODE_ADD_FIELD(fSTheta,         (0));
  SO_NODE_ADD_FIELD(fDTheta,         (FM_PI));
  SO_NODE_ADD_FIELD(nSides,          (10));
  SO_NODE_ADD_FIELD(alternateRep,    (NULL));

  children = new SoChildList(this);
}

// Destructor
SoEllipsoidSegment::~SoEllipsoidSegment() {
  delete children;
}

// GLRrender
void SoEllipsoidSegment::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  updateChildren();
  children->traverse(action);  
}

// generatePrimitives
void SoEllipsoidSegment::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

// getChildren
SoChildList* SoEllipsoidSegment::getChildren() const {
  return children;
}


// computeBBox
void SoEllipsoidSegment::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
  float Extent = FSQRT(FPOW(ax.getValue(),2) + 
                       FPOW(by.getValue(),2) + 
                       FPOW(cz.getValue(),2));
  SbVec3f vmin(-Extent,-Extent,-Extent), 
          vmax( Extent, Extent, Extent);
  TheCenter=SbVec3f(0,0,0);
  box.setBounds(vmin,vmax);
}

// updateChildren
void SoEllipsoidSegment::updateChildren() {
  assert(children->getLength()==1);
  SoSeparator* sep = (SoSeparator*)(*children)[0];
  SoCoordinate3* myCoords = (SoCoordinate3*)(sep->getChild(1));
  SoTriangleStripSet* myStrips = (SoTriangleStripSet*)(sep->getChild(2)); 

  SbBool flag = enableNotify(FALSE);

  int nS = nSides.getValue();

  float Stheta = fSTheta.getValue();
  float Dphi = (fDPhi.getValue())/(float)nS;
  float Dtheta = (fDTheta.getValue())/(float)nS;
  
  int ndim = 2 * nS * (nS + 1);
  SbVec3f* points = new SbVec3f[ndim];

  int i,j,jbeg;
  float theta, phi1, phi2;
  float r, x, y, z;
  float sphi1,cphi1,sphi2,cphi2,stheta,ctheta;
  float xeff,yeff,zeff;

  int ip = 0;  
  for (i=0; i < nS; i++){
    
    phi1 = fSPhi.getValue() + Dphi * (float) (i);
    sphi1 = FSIN(phi1);
    cphi1 = FCOS(phi1);

    phi2 = fSPhi.getValue() + Dphi * (float) (i+1);
    sphi2 = FSIN(phi2);
    cphi2 = FCOS(phi2);

    if(Stheta==0.F) {
      theta = Stheta;
      stheta = FSIN(theta);
      ctheta = FCOS(theta);      

      xeff = stheta * cphi1 / ax.getValue();   
      yeff = stheta * sphi1 / by.getValue();
      zeff = ctheta / cz.getValue();         
      r = 1.0F/FSQRT(xeff*xeff + yeff*yeff + zeff*zeff);
      x = r * stheta * cphi1;
      y = r * stheta * sphi1;
      z = r * ctheta;      
      points[ip].setValue(x,y,z); ip++;

      jbeg = 1;
    } else {
      jbeg = 0;
    }

    for(j=jbeg; j < (nS+1); j++) {
      
      theta = Stheta + Dtheta * (float) (j);
      stheta = FSIN(theta);
      ctheta = FCOS(theta);      

      xeff = stheta * cphi1 / ax.getValue();   
      yeff = stheta * sphi1 / by.getValue();
      zeff = ctheta / cz.getValue();         
      r = 1.0F/FSQRT(xeff*xeff + yeff*yeff + zeff*zeff);
      x = r * stheta * cphi1;
      y = r * stheta * sphi1;
      z = r * ctheta;      
      points[ip].setValue(x,y,z); ip++;

      xeff = stheta * cphi2 / ax.getValue();   
      yeff = stheta * sphi2 / by.getValue();
      zeff = ctheta / cz.getValue();         
      r = 1.0F/FSQRT(xeff*xeff + yeff*yeff + zeff*zeff);
      x = r * stheta * cphi2;
      y = r * stheta * sphi2;
      z = r * ctheta;      
      points[ip].setValue(x,y,z); ip++;
    }
  }
  
  myCoords -> point.setValues(0, ndim, points);
  
  delete [] points;

#ifdef INVENTOR2_0
  long* numVertices = new long[nS];
#else 
  int32_t*  numVertices = new int32_t[nS];
#endif
  int nvert = (Stheta==0.F) ? 2*nS+1 : 2*(nS+1);
  for (i=0; i<nS; i++)  numVertices[i] = nvert;
  myStrips -> numVertices.setValues(0, nS, numVertices);
  delete [] numVertices;

  enableNotify(flag);
}

// generateChildren
void SoEllipsoidSegment::generateChildren() {
  assert(children->getLength() ==0);

  SoSeparator* sep = new SoSeparator();

  SoShapeHints* myHints = new SoShapeHints;
  myHints -> vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  sep -> addChild(myHints);

  SoCoordinate3* myCoords = new SoCoordinate3;
  sep -> addChild(myCoords);

  SoTriangleStripSet* myStrips = new SoTriangleStripSet;
  sep -> addChild(myStrips);

  children->append(sep);
}

// generateAlternateRep
void SoEllipsoidSegment::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator*)(*children)[0]);
}

// clearAlternateRep
void SoEllipsoidSegment::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

  



