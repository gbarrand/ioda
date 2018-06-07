/*
 *  SoHatching.cxx
 *  
 *
 *  Created by Laurent Garnier on Jan 19 2004.
 * 
 * TODO : Gestion of non convex polygones given with a stripWith argument not null
 *        Actually, they are not draw correctly by the GL, we have to made a Tesselisation
 * to have them draw correctly
 */ 

// this :  
#include <HEPVis/nodes/SoHatching.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>

// HEPVis :
#include <HEPVis/SbMath.h>

#include <float.h> // for FLT_MIN
#include <vector>


SO_NODE_SOURCE(SoHatching)

void SoHatching::initClass() {
  SO_NODE_INIT_CLASS(SoHatching,SoShape,"Shape");
}

SoHatching::SoHatching()
{
  SO_NODE_CONSTRUCTOR(SoHatching);

  SO_NODE_ADD_FIELD(spacing,(0.1F));
  SO_NODE_ADD_FIELD(angle,(FM_PI/4));
  SO_NODE_ADD_FIELD(offset,(0.0F));
  SO_NODE_ADD_FIELD(stripWidth,(0.0F));

  cornerMinBB = SbVec3f(FLT_MAX,FLT_MAX,FLT_MAX);
  cornerMaxBB = SbVec3f(FLT_MIN,FLT_MIN,FLT_MIN);
}


SoHatching::~SoHatching() { 
}

// create a new bounding in XYZ plan
// list all the hatch present and redraw them in the new bounding box
int SoHatching::addPolyline(SbVec3f* Coord,int aNumber){

  fHatching.setSpacing(spacing.getValue());
  fHatching.setOffset(offset.getValue());
  fHatching.setAngle(angle.getValue());
  fHatching.setStripWidth(stripWidth.getValue());

  return fHatching.addPolyline(Coord,aNumber);
}
 
 
void SoHatching::generatePrimitives(
 SoAction* aAction
) 
{
  SbVec3f point, normal;
  
    //////////////////////////////////////////
    //----------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)  \
  point.setValue(x,y,z);                   \
  normal.setValue(nx,ny,nz);               \
  texCoord[0]=s;                         \
  texCoord[1]=t;                         \
  pv.setPoint(point);                      \
  pv.setNormal(normal);                    \
  pv.setTextureCoords(texCoord);           \
  shapeVertex(&pv);
  //----------------------------------------
  //////////////////////////////////////////

  SoPrimitiveVertex pa,pb;
  SbVec4f texCoord;
  if  (stripWidth.getValue() ==0) {   
#ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
    beginShape(aAction,POLYGON);
    endShape();
#endif
    const std::vector<SbVec3f>& listHatch = fHatching.getHatchPointsList();
    
    beginShape(aAction,POLYGON);
    for (unsigned int i=0;i<listHatch.size();i = i+2) {
      pa.setPoint(listHatch[i].getValue());
      pb.setPoint(listHatch[i+1].getValue());
      invokeLineSegmentCallbacks(aAction, &pa,&pb);
    } 
    endShape();
  }
  else {
#ifdef __COIN__ // To bypass a bug in invokeLineSegment when picking.
    beginShape(aAction,POLYGON);
    endShape();
#endif
    unsigned index =0;    
      for (unsigned int i=0;i<fHatching.getNumVertices();i++) {
        beginShape(aAction,POLYGON);
        for (unsigned int j=0;j<fHatching.getHatchNumVerticesList()[i];j++) {
          GEN_VERTEX(pa,fHatching.getHatchPointsList()[index+j].getValue()[0],fHatching.getHatchPointsList()[index+j].getValue()[1],fHatching.getHatchPointsList()[index+j].getValue()[2],0.0,0.0,fHatching.getNormal()[0],fHatching.getNormal()[1],fHatching.getNormal()[2]);
          //        pa.setPoint(fHatching.hatchPointsList()[index+j].getValue());
          //        shapeVertex(&pa);
        } 
        index +=fHatching.getHatchNumVerticesList()[i];
        endShape();
      }
  }
}

void SoHatching::computeBBox(SoAction*, SbBox3f& box, SbVec3f& center)
{
  box.setBounds(fHatching.getCornerMinBB(),fHatching.getCornerMaxBB());
  center = fHatching.getCornerMaxBB()-fHatching.getCornerMinBB();
}
