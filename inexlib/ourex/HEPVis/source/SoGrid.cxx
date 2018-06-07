// this :
#include <HEPVis/nodes/SoGrid.h>

//#define DEBUG
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

static void get_BBox(int,SbVec3f*,SbBox3f&,SbVec3f&);

SO_NODE_SOURCE(SoGrid)

void SoGrid::initClass() {
  SO_NODE_INIT_CLASS(SoGrid,SoShape,"Shape");
}

SoGrid::SoGrid(){
  SO_NODE_CONSTRUCTOR(SoGrid);
  SO_NODE_ADD_FIELD(position,(SbVec3f(0,0,0)));
  SO_NODE_ADD_FIELD(X,(SbVec3f(1,0,0)));
  SO_NODE_ADD_FIELD(Y,(SbVec3f(0,1,0)));
  SO_NODE_ADD_FIELD(rows,(1));
  SO_NODE_ADD_FIELD(columns,(1));
  SO_NODE_ADD_FIELD(width,(1));
  SO_NODE_ADD_FIELD(height,(1));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}

SoGrid::~SoGrid(){}

void SoGrid::generatePrimitives(SoAction* aAction) {
  SoPrimitiveVertex pv;
  SoState* state = aAction->getState();

  if(state && state->isElementEnabled
                (SoGLCacheContextElement::getClassStackIndex())) {
    SoGLCacheContextElement::shouldAutoCache(state,
      SoGLCacheContextElement::DO_AUTO_CACHE);
    //#if((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    //SoGLCacheContextElement::incNumShapes(state);
    //#endif
  }

  SbBool useTexFunction =
    (SoTextureCoordinateElement::getType(state) == 
     SoTextureCoordinateElement::FUNCTION);
  const SoTextureCoordinateElement* tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  } else {
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
  } else {                                 \
    texCoord[0]=s;                         \
    texCoord[1]=t;                         \
  }                                        \
  pv.setPoint(point);                      \
  pv.setNormal(normal);                    \
  pv.setTextureCoords(texCoord);           \
  shapeVertex(&pv);
  //----------------------------------------
  //////////////////////////////////////////

  int rown = rows.getValue();
  int coln = columns.getValue();
  float sx = width.getValue();
  float sy = height.getValue();

  if( (sx>0) && (sy>0) ) {

    float vx,vy,vz;
    position.getValue().getValue(vx,vy,vz);

    SbVec3f vX = X.getValue();
    SbVec3f vY = Y.getValue();
    SbVec3f vZ = vX.cross(vY);
    vX.normalize();
    vY.normalize();
    vZ.normalize();
    float Xx,Xy,Xz,Yx,Yy,Yz;
    vX.getValue(Xx,Xy,Xz);
    vY.getValue(Yx,Yy,Yz);
    
    float x0,y0,z0,x,y,z;
    for(int row=0;row<rown;row++) {
      for(int col=0;col<coln;col++) {
        //beginShape(aAction,TRIANGLE_FAN);  //It has a problem with Mesa.
        beginShape(aAction,TRIANGLE_STRIP);
        x = x0 = vx + Xx * row + Yx * col;
        y = y0 = vy + Xy * row + Yy * col;
        z = z0 = vz + Xz * row + Yz * col;
        GEN_VERTEX(pv,x,y,z, 0.0,0.0, vZ[0],vZ[1],vZ[2]);   
        x  = x0 + Xx * sx;
        y  = y0 + Xy * sx;
        z  = z0 + Xz * sx;
        GEN_VERTEX(pv,x,y,z, 0.0,0.0, vZ[0],vZ[1],vZ[2]);   
        x  = x0 + Yx * sy;
        y  = y0 + Yy * sy;
        z  = z0 + Yz * sy;
        GEN_VERTEX(pv,x,y,z, 0.0,0.0, vZ[0],vZ[1],vZ[2]);   
        x  = x0 + Xx * sx + Yx * sy;
        y  = y0 + Xy * sx + Yy * sy;
        z  = z0 + Xz * sx + Yz * sy;
        GEN_VERTEX(pv,x,y,z, 0.0,0.0, vZ[0],vZ[1],vZ[2]);   
        endShape();
      }
    }
  }
}

void SoGrid::computeBBox(SoAction*,SbBox3f& aBox,SbVec3f& aCenter){
  SbVec3f pos = position.getValue();

  float sx = width.getValue();
  float sy = height.getValue();

  SbVec3f points[4];
  points[0] = pos;

  int rown = rows.getValue();
  int coln = columns.getValue();
  SbVec3f vX = X.getValue();
  SbVec3f vY = Y.getValue();
  vX.normalize();
  vY.normalize();
  vX *= rown * sx;
  vY *= coln * sy;
  points[1] = pos + vX;
  points[2] = pos + vX + vY;
  points[3] = pos + vY;

  get_BBox(4,points,aBox,aCenter);
}

void get_BBox(int aNumber,SbVec3f* aPoints,SbBox3f& aBox,SbVec3f& aCenter){
  if(aNumber<=0) return;
  float xmin = 0.,ymin = 0.,zmin = 0.;
  float xmax = 0.,ymax = 0.,zmax = 0.;
  float xc = 0.,yc = 0.,zc = 0.;
  float x,y,z;

  int index = 0;
  for(int count=0;count<aNumber;count++){
    const SbVec3f& vec = aPoints[count];
    x = vec[0];
    y = vec[1];
    z = vec[2];
    if(count==0) {
      xmax = xmin = x;
      ymax = ymin = y;
      zmax = zmin = z;
    } else {
      xmin = SbMinimum(xmin,x);
      ymin = SbMinimum(ymin,y);
      zmin = SbMinimum(zmin,z);
      xmax = SbMaximum(xmax,x);
      ymax = SbMaximum(ymax,y);
      zmax = SbMaximum(zmax,z);
    }
    xc += x;
    yc += y;
    zc += z;
    index++;
  }
  aBox.setBounds(SbVec3f(xmin,ymin,zmin),SbVec3f(xmax,ymax,zmax));
  if(index!=0) {
    aCenter.setValue(xc/index,yc/index,zc/index);
  } else {
    aCenter.setValue(0.,0.,0.);
  }
}

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

void SoGrid::generateAlternateRep(SoAction*) {
  int rown = rows.getValue();
  int coln = columns.getValue();
  float sx = width.getValue();
  float sy = height.getValue();

  if( (rown<=0) || (coln<=0) ) return;
  if( (sx<=0) || (sy<=0) ) return;

  SoSeparator* separator = new SoSeparator;

  SoNormalBinding* normalBinding = new SoNormalBinding;
  normalBinding->value = SoNormalBinding::PER_FACE;
  separator->addChild(normalBinding);

  int nface = rown * coln;

  SbVec3f* points = new SbVec3f[nface*4];
  SbVec3f* normals = new SbVec3f[nface];
  int32_t* coordIndex = new int32_t[nface*5];

  float vx,vy,vz;
  position.getValue().getValue(vx,vy,vz);

  SbVec3f vX = X.getValue();
  SbVec3f vY = Y.getValue();
  SbVec3f vZ = vX.cross(vY);
  vX.normalize();
  vY.normalize();
  vZ.normalize();
  float Xx,Xy,Xz,Yx,Yy,Yz;
  vX.getValue(Xx,Xy,Xz);
  vY.getValue(Yx,Yy,Yz);
    
  int ipoint = 0;
  int icoord = 0;
  int inormal = 0;

  float x0,y0,z0,x,y,z;
  for(int row=0;row<rown;row++) {
    for(int col=0;col<coln;col++) {

      x = x0 = vx + Xx * row + Yx * col;
      y = y0 = vy + Xy * row + Yy * col;
      z = z0 = vz + Xz * row + Yz * col;
      points[ipoint+0].setValue(x,y,z);

      x  = x0 + Xx * sx;
      y  = y0 + Xy * sx;
      z  = z0 + Xz * sx;
      points[ipoint+1].setValue(x,y,z);

      x  = x0 + Xx * sx + Yx * sy;
      y  = y0 + Xy * sx + Yy * sy;
      z  = z0 + Xz * sx + Yz * sy;
      points[ipoint+2].setValue(x,y,z);

      x  = x0 + Yx * sy;
      y  = y0 + Yy * sy;
      z  = z0 + Yz * sy;
      points[ipoint+3].setValue(x,y,z);

      coordIndex[icoord+0] = ipoint + 0;
      coordIndex[icoord+1] = ipoint + 1;
      coordIndex[icoord+2] = ipoint + 2;
      coordIndex[icoord+3] = ipoint + 3;
      coordIndex[icoord+4] = SO_END_FACE_INDEX;

      ipoint += 4;
      icoord += 5;

      normals[inormal] = vZ;
      inormal++;
    }
  }

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  coordinate3->point.setValues(0,ipoint,points);
  separator->addChild(coordinate3);

  SoNormal* normal = new SoNormal;
  normal->vector.setValues(0,inormal,normals);
  separator->addChild(normal);

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  indexedFaceSet->coordIndex.setValues(0,icoord,coordIndex);
  separator->addChild(indexedFaceSet);

  delete [] coordIndex;
  delete [] points;
  delete [] normals;

  alternateRep.setValue(separator);

}

void SoGrid::clearAlternateRep() {alternateRep.setValue(NULL);}

void SoGrid::doAction(SoAction* aAction) {
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoShape::doAction(aAction);
}
