// this :
#include <HEPVis/nodes/SoPyramid.h>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>

#include <cmath>
#define FCOS(x)     ((float)cos(double(x)))
#define FSIN(x)     ((float)sin(double(x)))
#define FATAN(x)    ((float)atan(double(x)))

SO_NODE_SOURCE(SoPyramid)

void SoPyramid::initClass() {
  SO_NODE_INIT_CLASS(SoPyramid,SoShape,"Shape");
}

SoPyramid::SoPyramid() {
  SO_NODE_CONSTRUCTOR(SoPyramid);

  SO_NODE_ADD_FIELD(backWidth,(2.0));
  SO_NODE_ADD_FIELD(backHeight,(2.0));
  SO_NODE_ADD_FIELD(frontWidth,(2.0));
  SO_NODE_ADD_FIELD(frontHeight,(2.0));
  SO_NODE_ADD_FIELD(depth,(2.0));
}
SoPyramid::~SoPyramid() {}

void SoPyramid::generatePrimitives(SoAction* aAction) {
  // This variable is used to store each vertex
  SoPrimitiveVertex pv;

  // Access the stat from the action
  SoState *state = aAction->getState();

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
  float bw2 = backWidth.getValue()*0.5F;
  float bh2 = backHeight.getValue()*0.5F;
  float fw2 = frontWidth.getValue()*0.5F;
  float fh2 = frontHeight.getValue()*0.5F;
  float d2 = depth.getValue()*0.5F;

  float points[NPOINTS][3];
  points[0][0] =  bw2; 
  points[0][1] =  bh2;
  points[0][2] = -d2;

  points[1][0] = -bw2;
  points[1][1] =  bh2;
  points[1][2] = -d2;

  points[2][0] = -bw2; 
  points[2][1] = -bh2;
  points[2][2] = -d2;

  points[3][0] =  bw2;
  points[3][1] = -bh2;
  points[3][2] = -d2;

  points[4][0] =  fw2; 
  points[4][1] =  fh2; 
  points[4][2] =  d2;

  points[5][0] = -fw2; 
  points[5][1] =  fh2; 
  points[5][2] =  d2;

  points[6][0] = -fw2; 
  points[6][1] = -fh2; 
  points[6][2] =  d2;

  points[7][0] =  fw2; 
  points[7][1] = -fh2; 
  points[7][2] =  d2;

  float t1  = FATAN((fw2-bw2)/(2*d2));
  float t2  = FATAN((fh2-bh2)/(2*d2));
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
    beginShape(aAction,TRIANGLE_FAN);
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

#define MAXIMUM(a,b) ((a)>(b)?(a):(b))

void SoPyramid::computeBBox(SoAction*,SbBox3f& aBox,SbVec3f& aCenter){
  float w2 = MAXIMUM(backWidth.getValue(),frontWidth.getValue())*0.5F;
  float h2 = MAXIMUM(backHeight.getValue(),frontHeight.getValue())*0.5F;
  float d2 = depth.getValue()*0.5F;

  aBox.setBounds(SbVec3f(-w2,-h2,-d2),SbVec3f(w2,h2,d2));
  aCenter.setValue(0,0,0);
}
