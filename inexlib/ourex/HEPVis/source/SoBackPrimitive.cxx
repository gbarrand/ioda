// this :
#include <HEPVis/nodes/SoBackPrimitive.h>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>

#include <HEPVis/SbMath.h>

// This statement is required
SO_NODE_SOURCE(SoBackPrimitive)

// initClass
void SoBackPrimitive::initClass(){
  SO_NODE_INIT_CLASS(SoBackPrimitive,SoShape,"Shape");
}
// Constructor
SoBackPrimitive::SoBackPrimitive() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoBackPrimitive);
  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(dx1,(1.0));
  SO_NODE_ADD_FIELD(dx2,(1.0));
  SO_NODE_ADD_FIELD(dy1,(1.0));
  SO_NODE_ADD_FIELD(dy2,(1.0));
  SO_NODE_ADD_FIELD(dz,(1.0));
}
// Destructor
SoBackPrimitive::~SoBackPrimitive() {
}
// generatePrimitives
void SoBackPrimitive::generatePrimitives(SoAction *action) {
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
  const SoTextureCoordinateElement *tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  }
  else {
    texCoord[2] = 0.0;
    texCoord[3] = 1.0;
  }
  SbVec3f point, normal;


  //////////////////////////////////////////
  //----------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)  \
  point.setValue(x,y,z);                   \
  normal.setValue(nx,ny,nz);               \
  if (useTexFunction) {                    \
    texCoord=tce->get(point,normal);       \
  }                                        \
  else {                                   \
    texCoord[0]=s;                         \
    texCoord[1]=t;                         \
  }                                        \
  pv.setPoint(point);                      \
  pv.setNormal(normal);                    \
  pv.setTextureCoords(texCoord);           \
  shapeVertex(&pv);
  //----------------------------------------
  //////////////////////////////////////////

  const int NPOINTS=8, NFACES=6, NINDICES = NFACES*5;
  int indices[NINDICES] = {3,2,1,0, SO_END_FACE_INDEX,  //z back.
                           4,5,6,7, SO_END_FACE_INDEX,  //z front.
                           0,1,5,4, SO_END_FACE_INDEX,  //y up.
                           1,2,6,5, SO_END_FACE_INDEX,  //x left.
                           2,3,7,6, SO_END_FACE_INDEX,  //y down.
                           3,0,4,7, SO_END_FACE_INDEX}; //x right.

  
  // points for the eight vertices
  float points[NPOINTS][3];
  points[0][0] =  dx1.getValue(); 
  points[0][1] =  dy1.getValue(); 
  points[0][2] = -dz.getValue();

  points[1][0] = -dx1.getValue();
  points[1][1] =  dy1.getValue();
  points[1][2] = -dz.getValue();

  points[2][0] = -dx1.getValue(); 
  points[2][1] = -dy1.getValue(); 
  points[2][2] = -dz.getValue();

  points[3][0] =  dx1.getValue(); 
  points[3][1] = -dy1.getValue(); 
  points[3][2] = -dz.getValue();

  points[4][0] =  dx2.getValue(); 
  points[4][1] =  dy2.getValue(); 
  points[4][2] =  dz.getValue();

  points[5][0] = -dx2.getValue(); 
  points[5][1] =  dy2.getValue(); 
  points[5][2] =  dz.getValue();

  points[6][0] = -dx2.getValue(); 
  points[6][1] = -dy2.getValue(); 
  points[6][2] =  dz.getValue();

  points[7][0] =  dx2.getValue(); 
  points[7][1] = -dy2.getValue(); 
  points[7][2] =  dz.getValue();

  float t1  = FATAN((dx2.getValue()-dx1.getValue())/(2*dz.getValue()));
  float t2  = FATAN((dy2.getValue()-dy1.getValue())/(2*dz.getValue()));
  float st1 = FSIN(t1);
  float st2 = FSIN(t2);
  float ct1 = FCOS(t1);
  float ct2 = FCOS(t2);

  float normals[NFACES][3];
  //z back.
  normals[0][0] = 0   ; normals[0][1] =    0; normals [0][2] =  -1;    
  //z front.
  normals[1][0] = 0   ; normals[1][1] =    0; normals [1][2] =   1;    
  //y up.
  normals[2][0] = 0   ; normals[2][1] =  ct2; normals [2][2] = -st2;    
  //x left.
  normals[3][0] = -ct1; normals[3][1] =    0; normals [3][2] = -st1;    
  //y down.
  normals[4][0] = 0   ; normals[4][1] = -ct2; normals [4][2] = -st2;    
  //x right.
  normals[5][0] =  ct1; normals[5][1] =    0; normals [5][2] = -st1;    

  float x,y,z;
  int   index;
  for (int nf=0;nf<NFACES;nf++) {
    beginShape(action,TRIANGLE_FAN);
    index = indices[nf * 5];   
    x = points[index][0];
    y = points[index][1];
    z = points[index][2];
    GEN_VERTEX(pv,x,y,z,0.0,0.0,normals[nf][0],normals[nf][1],normals[nf][2]);   
    index = indices[nf * 5 + 1];   
    x = points[index][0];
    y = points[index][1];
    z = points[index][2];
    GEN_VERTEX(pv,x,y,z,0.0,0.0,normals[nf][0],normals[nf][1],normals[nf][2]);   
    index = indices[nf * 5 + 2];   
    x = points[index][0];
    y = points[index][1];
    z = points[index][2];
    GEN_VERTEX(pv,x,y,z,0.0,0.0,normals[nf][0],normals[nf][1],normals[nf][2]);   
    index = indices[nf * 5 + 3];   
    x = points[index][0];
    y = points[index][1];
    z = points[index][2];
    GEN_VERTEX(pv,x,y,z,0.0,0.0,normals[nf][0],normals[nf][1],normals[nf][2]);   
    endShape();
  }
}
// computeBBox
void SoBackPrimitive::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  float fDx = dx1.getValue();
  float fDy = dy1.getValue();
  
  if (dx2.getValue() > fDx) fDx = dx2.getValue(); 
  if (dy2.getValue() > fDy) fDy = dy2.getValue(); 

  SbVec3f vmin(-fDx,-fDy,-dz.getValue()), 
          vmax( fDx, fDy, dz.getValue());

  center.setValue(0,0,0);
  box.setBounds(vmin,vmax);
}
