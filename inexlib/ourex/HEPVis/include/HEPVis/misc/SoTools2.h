#ifndef HEPVis_SoTools2_h
#define HEPVis_SoTools2_h

// for SoAxis and SoPlotter.

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>

inline int SoTools_SbFROUND(float aFloat) {
  // From CoinGL/src/base/SbViewportRegion.cpp.
  if (aFloat == (float) (int(aFloat))) return int(aFloat);
  else return (aFloat>0.0f) ? int(aFloat+0.5f) : -int(0.5f-aFloat);
}

inline SbBool SoTools_IsTTF(const SbString& aString){
  if(aString.getLength()<4) return FALSE;
  if(aString[0]!='T') return FALSE;
  if(aString[1]!='T') return FALSE;
  if(aString[2]!='F') return FALSE;
  if(aString[3]!='/') return FALSE;
  return TRUE;
}

  enum SoTools_HorizontalJustification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  enum SoTools_VerticalJustification {
    BOTTOM = 0x00,
    MIDDLE = 0x01,
    TOP    = 0x02
  };

inline void SoTools_addAlignedStringToNode (
 SoSeparator* aSeparator 
,const SbString& aFont
,const SbString& aEncoding
,SbBool aSmoothing
,const SbString& aString  
,const SbVec3f& aVertex
,const SbVec3f& aX
,const SbVec3f& aY
,float aSize    
,SoTools_HorizontalJustification aHJust
,SoTools_VerticalJustification aVJust
,SbBool aRotated = FALSE
){
  if(aString=="") return;

  if ((aFont.getLength() >=6 ) && (aFont.getSubString(0,5)=="Text3/")) {
    SoSeparator* separator = new SoSeparator;
    aSeparator->addChild(separator);

    SoFont* font = new SoFont;
    font->name.setValue(aFont.getSubString(6,aFont.getLength()-1));
    font->size.setValue(1);
    separator->addChild(font);
    
    SoTranslation* transform = new SoTranslation;
    transform->translation.setValue(aVertex);
    separator->addChild(transform);

    SoText3* text = new SoText3;
    text->string.setValue(aString);
    separator->addChild(text);

  } else if((aFont.getLength() >=6 ) && (aFont.getSubString(0,5)=="Text2/")) {
    SoSeparator* separator = new SoSeparator;
    aSeparator->addChild(separator);

    SoFont* font = new SoFont;
    font->name.setValue(aFont.getSubString(6,aFont.getLength()-1));
    font->size.setValue(10);
    separator->addChild(font);
    
    SoText2* text = new SoText2;
    text->string.setValue (aString);
    separator->addChild(text);

  } else if(SoTools_IsTTF(aFont)==TRUE) {
    SoSeparator* separator = new SoSeparator;
    aSeparator->addChild(separator);

    SoTranslation* transform = new SoTranslation;
    transform->translation.setValue(aVertex);
    separator->addChild(transform);

    SoTextTTF* text = new SoTextTTF;
    text->fontName.setValue(aFont.getSubString(4,aFont.getLength()-1));
    text->encoding.setValue(aEncoding);
    text->smoothing.setValue(aSmoothing);
    // Majic factor to have same aspect than SoTextHershey.
    float majic = 290.0F; //To match PAW look and feel.
    text->size.setValue(SoTools_SbFROUND(aSize * majic)); // around 10
    text->string.setValue(aString);
    text->rotated.setValue(aRotated);
    text->horizontalJustification.setValue(aHJust);
    text->verticalJustification.setValue(aVJust);
    separator->addChild(text);

  } else {  // Default is Hershey.
    SoSeparator* separator = new SoSeparator;
    aSeparator->addChild(separator);

    SbVec3f X = aX;
    SbVec3f Y = aY;
    X.normalize ();
    Y.normalize ();
    SbVec3f Z = X.cross (Y);
    Y = Z.cross (X);
    // now X,Y,Z normalized frame
    SbMatrix P(X[0],X[1],X[2],0,
               Y[0],Y[1],Y[2],0,
               Z[0],Z[1],Z[2],0,
               0,0,0,1);
    SbMatrix scale;
    scale.setScale(SbVec3f(aSize,aSize,1));
    SbMatrix amatrix;
    amatrix.makeIdentity();
    amatrix.multLeft(P);     //Second
    amatrix.multLeft(scale); //First

    SoTransform* transform = new SoTransform;
    SbMatrix matrix;
    matrix.setTranslate(aVertex);   //Second. Here last.
    transform->multLeft(matrix);
    transform->multLeft(amatrix);   //First 
    separator->addChild(transform);

    SoTextHershey* text = new SoTextHershey;
    text->encoding.setValue(aEncoding);
    text->string.setValue(aString);
    //text->height.setValue(1.4F);
    text->horizontalJustification.setValue(aHJust);
    text->verticalJustification.setValue(aVJust);
    separator->addChild(text);
  }
}

/////////////////////////////////
///For SoPlotter ////////////////
/////////////////////////////////
#include <Inventor/nodes/SoCoordinate3.h>
#include <HEPVis/nodes/SoMarkerSet.h>

inline void SoTools_addPointsToNode (
 SoSeparator* aSeparator
,int aNumber
,SbVec3f* aPoints
,SbMarkerStyle aStyle
,int aSize
){
  if(aNumber<=0) return;
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aPoints);
  aSeparator->addChild(coordinate3);
  if(aStyle==SbMarkerDot) {
    SoPointSet* pointSet = new SoPointSet;
    pointSet->numPoints.setValue(pointn);
    aSeparator->addChild(pointSet);
  } else {
    HEPVis_SoMarkerSet* markerSet = new HEPVis_SoMarkerSet;
    markerSet->markerIndex.setValue
      (HEPVis_SoMarkerSet::getMarkerIndex(aStyle,aSize));
    markerSet->numPoints.setValue(pointn);
    aSeparator->addChild(markerSet);
  }
}

#include <Inventor/nodes/SoLineSet.h>

inline void SoTools_addLinesToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=0) return;
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,&pointn);
  aSeparator->addChild(lineSet);
}

inline void SoTools_addSegmentsToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  int segmentn = pointn/2;
  int32_t* vertices = new int32_t [segmentn];
  for (int count=0;count<segmentn;count++) vertices[count] = 2;

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,segmentn,vertices);
  aSeparator->addChild(lineSet);

  delete [] vertices;
}

#include <Inventor/nodes/SoFaceSet.h>

inline void SoTools_addPolygonToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=0) return;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  //SoEdgedFaceSet* faceSet = new SoEdgedFaceSet;
  SoFaceSet* faceSet = new SoFaceSet;
  faceSet->numVertices.setValues(0,1,&pointn);
  aSeparator->addChild(faceSet);
}

#include <Inventor/nodes/SoIndexedNurbsCurve.h>

inline void SoTools_addNurbsCurveToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return; //Need at least two points.

  SbVec3f* tangents = new SbVec3f[aNumber];
  tangents[0] = aLine[1] - aLine[0];
  for(int i = 1;i<(aNumber-1);i++) tangents[i] = aLine[i+1] - aLine[i-1];
  tangents[aNumber-1] = aLine[aNumber-1] - aLine[aNumber-2];

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  aSeparator->addChild(coordinate3);

  float knots[8] = {0,0,0,0,1,1,1,1};

  SbVec3f points[4];
  int icoord = 0;
  int32_t coordIndex[5];
  for(int index=0;index<(aNumber-1);index++) {
    const SbVec3f& a = aLine[index];
    const SbVec3f& b = aLine[index+1];
    const SbVec3f& ta = tangents[index];
    const SbVec3f& tb = tangents[index+1];

    points[0] = a;
    points[1] = a + ta * 0.1F;
    points[2] = b - tb * 0.1F;
    points[3] = b;

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = -1; //End of list.

    coordinate3->point.setValues(icoord,4,points);
    icoord += 4;

    SoIndexedNurbsCurve* nurbsCurve = new SoIndexedNurbsCurve;
    nurbsCurve->numControlPoints.setValue(4);
    nurbsCurve->knotVector.setValues(0,8,knots);
    nurbsCurve->coordIndex.setValues(0,5,coordIndex);
    aSeparator->addChild(nurbsCurve);

  }

  delete [] tangents;
}

#include <Inventor/nodes/SoNormal.h>

inline void SoTools_addBoxCoords(SoSeparator& aSeparator,float a_w,float a_h,float a_d){
  SbVec3f points[24];
  SbVec3f normals[6];

  float dw =  a_w*0.5F;
  float dh =  a_h*0.5F;
  float dd =  a_d*0.5F;

  //bottom-y :
 {float yy = -a_h*0.5F;
  points[0].setValue(-dw, yy, dd);
  points[1].setValue(-dw, yy,-dd);
  points[2].setValue( dw, yy,-dd);
  points[3].setValue( dw, yy, dd);
  normals[0].setValue(0,-1,0);}

  //top-y :
 {float yy =  a_h*0.5F;
  points[4].setValue(-dw, yy, dd);
  points[5].setValue( dw, yy, dd);
  points[6].setValue( dw, yy,-dd);
  points[7].setValue(-dw, yy,-dd);
  normals[1].setValue(0,1,0);}

  //front-x :
 {float xx =  a_w*0.5F;
  points[8].setValue(xx,-dh,-dd);
  points[9].setValue(xx, dh,-dd);
  points[10].setValue(xx, dh, dd);
  points[11].setValue(xx,-dh, dd);
  normals[2].setValue(1,0,0);}

  //back-x :
 {float xx = -a_w*0.5F;
  points[12].setValue(xx,-dh,-dd);
  points[13].setValue(xx,-dh, dd);
  points[14].setValue(xx, dh, dd);
  points[15].setValue(xx, dh,-dd);
  normals[3].setValue(-1,0,0);}

  //back-z :
 {float zz = -a_d*0.5F;
  points[16].setValue(-dw,-dh, zz);
  points[17].setValue(-dw, dh, zz);
  points[18].setValue( dw, dh, zz);
  points[19].setValue( dw,-dh, zz);
  normals[4].setValue(0,0,-1);}

  //front-z :
 {float zz =  a_d*0.5F;
  points[20].setValue(-dw,-dh, zz);
  points[21].setValue( dw,-dh, zz);
  points[22].setValue( dw, dh, zz);
  points[23].setValue(-dw, dh, zz);
  normals[5].setValue(0,0,1);}

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = 24;
  coordinate3->point.setValues(0,pointn,points);
  aSeparator.addChild(coordinate3);

  SoNormal* normal = new SoNormal;
  normal->vector.setValues(0,6,normals);
  aSeparator.addChild(normal);
}

#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/nodes/SoMaterial.h>

inline SbBool SoTools_setDraggerColor(SoTranslate2Dragger& aDragger,const SbColor& aColor){
  SoNode* node = aDragger.getPart("translator",FALSE);
  if(!node) return FALSE;
  if(node->isOfType(SoSeparator::getClassTypeId())==FALSE) return FALSE;
  SoSeparator* s = (SoSeparator*)node;
  int num = s->getNumChildren();
  if(!num) return FALSE;
  SoNode* n = s->getChild(0);
  if(!n) return FALSE;
  if(n->isOfType(SoMaterial::getClassTypeId())==FALSE) return FALSE;
  ((SoMaterial*)n)->diffuseColor.setValue(aColor);
  return TRUE;
}

#endif
