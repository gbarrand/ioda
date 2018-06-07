/*-----------------------------HEPVis---------------------------------------*/
/*                                                                          */
/* Node:             SoTubs                                                 */
/* Description:      Represents the G4Tubs Geant Geometry entity            */
/* Author:           Joe Boudreau Nov 11 1996                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

// this :
#include <HEPVis/nodes/SoTubs.h>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

//
//! help with trigonometry.  increments sines an cosines by an angle.
//
void inc(float & sinPhi, float & cosPhi, float sinDeltaPhi, float cosDeltaPhi){
  float oldSin=sinPhi,oldCos=cosPhi;
  sinPhi = oldSin*cosDeltaPhi+oldCos*sinDeltaPhi;
  cosPhi = oldCos*cosDeltaPhi-oldSin*sinDeltaPhi;    
}


// This statement is required
SO_NODE_SOURCE(SoTubs)

// Constructor
SoTubs::SoTubs() {


  // This statement is required
  SO_NODE_CONSTRUCTOR(SoTubs);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(pRMin,               (0));
  SO_NODE_ADD_FIELD(pRMax,               (1));
  SO_NODE_ADD_FIELD(pDz,                 (10));
  SO_NODE_ADD_FIELD(pSPhi,               (0));
  SO_NODE_ADD_FIELD(pDPhi,               (2*FM_PI));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  children = new SoChildList(this);
}

// Destructor
SoTubs::~SoTubs() {
  delete children;
}


// initClass
void SoTubs::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoTubs,SoShape,"Shape");
}

// generatePrimitives
void SoTubs::generatePrimitives(SoAction *action) {
  // This variable is used to store each vertex
  SoPrimitiveVertex pv;

  // Access the stat from the action
  SoState *state = action->getState();

  // See if we have to use a texture coordinate function,
  // rather than generating explicit texture coordinates.
  SbBool useTexFunction=
    (SoTextureCoordinateElement::getType(state) == 
     SoTextureCoordinateElement::FUNCTION);

  // If we need to generate texture coordinates with a function,
  // we'll need an SoGLTextureCoordinateElement.  Otherwise, we'll
  // set up the coordinates directly.
  const SoTextureCoordinateElement* tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  }
  else {
    texCoord[2] = 0;
    texCoord[3] = 1;
  }
  SbVec3f point, normal;


  ///////////////////////////////////////////////////////
  //-----------------------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)               \
  point.setValue((float)(x),(float)(y),(float)(z));     \
  normal.setValue((float)(nx),(float)(ny),(float)(nz)); \
  if (useTexFunction) {                                 \
    texCoord=tce->get(point,normal);                    \
  } else {                                              \
    texCoord[0]=(float)(s);                             \
    texCoord[1]=(float)(t);                             \
  }                                                     \
  pv.setPoint(point);                                   \
  pv.setNormal(normal);                                 \
  pv.setTextureCoords(texCoord);                        \
  shapeVertex(&pv);
  //-----------------------------------------------------
  ///////////////////////////////////////////////////////

  int NPHI = (int)(2+94*FFABS(pDPhi.getValue()/(2*FM_PI)));
  float deltaPhi = pDPhi.getValue()/NPHI;
  float phi0 = pSPhi.getValue();
  float phi1 = phi0+pDPhi.getValue();

  float rMax = pRMax.getValue();
  float rMin = pRMin.getValue();
  float zMax = pDz.getValue();
  float zMin = -zMax;
  float cosPhi0 = FCOS(phi0);
  float sinPhi0 = FSIN(phi0);
  float cosPhi1 = FCOS(phi1);
  float sinPhi1 = FSIN(phi1);
  float cosDeltaPhi = FCOS(deltaPhi);
  float sinDeltaPhi = FSIN(deltaPhi);
  //
  // The outer surface!
  //
  int i;
  float sinPhi,cosPhi;
  beginShape(action,TRIANGLE_STRIP);
  sinPhi=sinPhi0;
  cosPhi=cosPhi0;
  for (i = 0; i<=NPHI; i++) {
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,0,0,cosPhi,sinPhi,0);   
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1,1,cosPhi,sinPhi,0);   
    inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
  }
  endShape();
  //
  // The inner surface!
  //
  if(rMin!=0) {
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,0,0,-cosPhi,-sinPhi,0);   
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,1,1,-cosPhi,-sinPhi,0);   
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
    } 
    endShape();
  }
  if (FFABS(deltaPhi)<2*FM_PI) { 
    //
    // The end 
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,0,0,sinPhi,-cosPhi,0);   
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1,1,sinPhi,-cosPhi,0);   
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,1,0,sinPhi,-cosPhi,0);   
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,0,1,sinPhi,-cosPhi,0);   
    endShape();
    //
    // The other end 
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi1;
    cosPhi=cosPhi1;
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi, zMax,0,0,-sinPhi,+cosPhi,0);   
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi, zMin,1,1,-sinPhi,+cosPhi,0);   
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi, zMax,1,0,-sinPhi,+cosPhi,0);   
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi, zMin,0,1,-sinPhi,+cosPhi,0);   
    endShape();
  }
  //
  // The outer surface at z=+PDZ
  //
  if(rMin==0) {
    beginShape(action,TRIANGLE_FAN);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,0,0,zMax,0,0,0,0,1);   
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,1,1,0,0,1);   
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
    }
    endShape();
    //
    // The outer surface at z=-PDZ
    //
    beginShape(action,TRIANGLE_FAN);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,0,0,zMin,0,0,0,0,-1);   
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1,1,0,0,-1);   
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
    }
    endShape();
  } else {
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,0,0,0,0,1);   
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,1,1,0,0,1);   
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
    }
    endShape();
    //
    // The outer surface at z=-PDZ
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,0,0,0,0,-1);   
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1,1,0,0,-1);   
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);    
    }
    endShape();
  }
}

// getChildren
SoChildList *SoTubs::getChildren() const {
  return children;
}


// computeBBox
void SoTubs::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  SbVec3f vmin(-pRMax.getValue(),-pRMax.getValue(),-pDz.getValue()), 
          vmax( pRMax.getValue(), pRMax.getValue(), pDz.getValue());
  center.setValue(0,0,0);
  box.setBounds(vmin,vmax);
}


// updateChildren
void SoTubs::updateChildren() {

  // Redraw the G4Tubs....

  if(children->getLength()!=1) return;
  SoSeparator       *sep                = (SoSeparator *)  ( *children)[0];
  SoCoordinate3     *theCoordinates     = (SoCoordinate3 *)      ( sep->getChild(0));
  SoNormal          *theNormals         = (SoNormal *)           ( sep->getChild(1)); 
  SoNormalBinding   *theNormalBinding   = (SoNormalBinding *)    ( sep->getChild(2));
  SoIndexedFaceSet  *theFaceSet         = (SoIndexedFaceSet *)   ( sep->getChild(3));
  
  
  const int NPHI=96, NPOINTS=2*(2*NPHI+2), NFACES=4*NPHI+2, NINDICES = NFACES*5;
#ifdef INVENTOR2_0
  static long     indices[NINDICES];
#else
  static int32_t  indices[NINDICES];
#endif
    
  static int init=0;
  // Indices need to be generated once! This is here to keep it 
  // close to the point generation, since otherwise it will be confusing.
    
  int i;
  if (!init) {
    init = 1;
    // Outer face
    for (i = 0; i< NPHI; i++) {
      // 0 1 3 2;
      indices[5*i+0] =  2*i+0;
      indices[5*i+1] =  2*i+1;
      indices[5*i+2] =  2*i+3;
      indices[5*i+3] =  2*i+2;
      indices[5*i+4] = SO_END_FACE_INDEX;
    }
    // the inner face
    for (i=0;i<NPHI;i++) {
      indices[5*1*NPHI + 5*i+0] = 2*NPHI+2 + 2*i+0;  
      indices[5*1*NPHI + 5*i+1] = 2*NPHI+2 + 2*i+1;
      indices[5*1*NPHI + 5*i+2] = 2*NPHI+2 + 2*i+3;
      indices[5*1*NPHI + 5*i+3] = 2*NPHI+2 + 2*i+2;
      indices[5*1*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the top side
    for (i=0;i<NPHI;i++) {
      indices[5*2*NPHI + 5*i+0] = 2*i+0;
      indices[5*2*NPHI + 5*i+1] = 2*i+2;
      indices[5*2*NPHI + 5*i+2] = NPOINTS - (2*i+4);
      indices[5*2*NPHI + 5*i+3] = NPOINTS - (2*i+2);
      indices[5*2*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the bottom side
    for (i=0;i<NPHI;i++) {
      indices[5*3*NPHI + 5*i+0] = 2*i+1;
      indices[5*3*NPHI + 5*i+1] = NPOINTS - (2*i+1);
      indices[5*3*NPHI + 5*i+2] = NPOINTS - (2*i+3);
      indices[5*3*NPHI + 5*i+3] = 2*i+3;
      indices[5*3*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the odd side
    indices[5*4*NPHI +0] = 2*NPHI;
    indices[5*4*NPHI +1] = 2*NPHI+1;
    indices[5*4*NPHI +2] = 2*NPHI+3;
    indices[5*4*NPHI +3] = 2*NPHI+2;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = 0;
    indices[5*4*NPHI +5 +1] = NPOINTS-2;
    indices[5*4*NPHI +5 +2] = NPOINTS-1;
    indices[5*4*NPHI +5 +3] = 1;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  // Points need to be generated each time:
  if (pDPhi.getValue()<2*FM_PI) {
    // the odd side
    indices[5*4*NPHI +0] = 2*NPHI;
    indices[5*4*NPHI +1] = 2*NPHI+1;
    indices[5*4*NPHI +2] = 2*NPHI+3;
    indices[5*4*NPHI +3] = 2*NPHI+2;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = 0;
    indices[5*4*NPHI +5 +1] = NPOINTS-2;
    indices[5*4*NPHI +5 +2] = NPOINTS-1;
    indices[5*4*NPHI +5 +3] = 1;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  } 
  else {
    // the odd side
    indices[5*4*NPHI +0] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +1] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +2] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +3] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +1] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +2] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +3] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  // The outer surface
  float DeltaPhi = pDPhi.getValue()/NPHI;
  float phi = pSPhi.getValue();
  float pp;
  float points[NPOINTS][3],normals[NFACES][3];
  for (i = 0; i<=NPHI; i++) {
    points[2*i+0][0] = pRMax.getValue()*FCOS(phi); 
    points[2*i+0][1] = pRMax.getValue()*FSIN(phi); 
    points[2*i+0][2] = +pDz.getValue();

    points[2*i+1][0] = pRMax.getValue()*FCOS(phi); 
    points[2*i+1][1] = pRMax.getValue()*FSIN(phi); 
    points[2*i+1][2] = -pDz.getValue();

    pp = phi+DeltaPhi/2;
    if (i!=NPHI) {
      normals[i][0] = FCOS(pp); 
      normals[i][1] = FSIN(pp); 
      normals[i][2] = 0;
    }
    phi+=DeltaPhi;
  }
  // The inner surface
  phi = pSPhi.getValue() + pDPhi.getValue();
  for (i = 0; i<=NPHI; i++) {
    points[2*NPHI+2+2*i+0][0] = pRMin.getValue()*FCOS(phi); 
    points[2*NPHI+2+2*i+0][1] = pRMin.getValue()*FSIN(phi); 
    points[2*NPHI+2+2*i+0][2] = +pDz.getValue();

    points[2*NPHI+2+2*i+1][0] = pRMin.getValue()*FCOS(phi); 
    points[2*NPHI+2+2*i+1][1] = pRMin.getValue()*FSIN(phi); 
    points[2*NPHI+2+2*i+1][2] = -pDz.getValue();
    pp = phi-DeltaPhi/2;
    if (i!=NPHI) {
      normals[NPHI+i][0] = -FCOS(pp); 
      normals[NPHI+i][1] = -FSIN(pp); 
      normals[NPHI+i][2] = 0;
    }
    phi-=DeltaPhi;
  }
  // The top side
  for (i=0;i<NPHI;i++) {
    normals[2*NPHI+i][0]=normals[2*NPHI+i][1]=0; 
    normals[2*NPHI+i][2]=  1;
  } 
  // The bottom side
  for (i=0;i<NPHI;i++) {
    normals[3*NPHI+i][0]=normals[3*NPHI+i][1]=0; 
    normals[3*NPHI+i][2]= -1;
  } 
  // The odd side
  phi = pSPhi.getValue(); 
  normals[4*NPHI+0][0]=  FSIN(phi); 
  normals[4*NPHI+0][1]= -FCOS(phi); 
  normals[4*NPHI+0][2]=0;
    
    // Another odd side
  phi = pSPhi.getValue()+pDPhi.getValue(); 
  normals[4*NPHI+1][0]= -FSIN(phi); 
  normals[4*NPHI+1][1]= +FCOS(phi); 
  normals[4*NPHI+1][2]=0;

  theCoordinates->point.setValues(0,NPOINTS,points);

  theFaceSet->coordIndex.setValues(0,NINDICES,indices);

  theNormals->vector.setValues(0,NFACES,normals);

  theNormalBinding->value=SoNormalBinding::PER_FACE;
}

// generateChildren
void SoTubs::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine. 

  if(children->getLength() !=0) return;
  SoSeparator      *sep              = new SoSeparator(); 
  SoCoordinate3    *theCoordinates   = new SoCoordinate3();
  SoNormal         *theNormals       = new SoNormal(); 
  SoNormalBinding  *theNormalBinding = new SoNormalBinding();
  SoIndexedFaceSet *theFaceSet       = new SoIndexedFaceSet();
  // 
  // This line costs some in render quality! but gives speed.
  // 
  sep->addChild(theCoordinates);
  sep->addChild(theNormals);
  sep->addChild(theNormalBinding);
  sep->addChild(theFaceSet);
  children->append(sep);
}

// generateAlternateRep
void SoTubs::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoTubs::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

void SoTubs::doAction(SoAction* action) {
  SO_ALTERNATEREP_DO_ACTION(action)
  SoShape::doAction(action);
}

#include <HEPVis/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
void SoTubs::write(
 SoWriteAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    // enforce the writing of fields, else a pure Coin reader
    // (not linked to HEPVis libs) produces warnings.

    HEPVIS_SET_FIELDS_DEFAULT(FALSE)

    SoShape::write(aAction);

    HEPVIS_RESTORE_FIELDS_DEFAULT

    // other solution :
    //altRep->write(aAction);
  } else {
    SoShape::write(aAction);
  }
}
