// this :
#include <HEPVis/misc/SoTools.h>

// Inventor :
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoIndexedNurbsCurve.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/actions/SoWriteAction.h>

// for manip :
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/actions/SoSearchAction.h>

#define HEPVIS_HAS_VRML2
#ifdef HEPVIS_HAS_VRML2
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#endif

// HEPVisUtils :
#include <HEPVis/SbString.h>
#include <HEPVis/SbConvert.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/SbStyle.h>
#include <HEPVis/nodes/SoSceneGraph.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodes/SoHighlightMaterial.h>
#include <HEPVis/nodes/SoSplineCurve.h>
#include <HEPVis/nodes/SoMarkerSet.h>
#include <HEPVis/actions/SoAlternateRepAction.h>
#include <HEPVis/actions/SoCounterAction.h>

#include "iv2pov.h"

#include <cstring> //strrchr

SoPathList* SoTools::getSceneGraphs(SoNode* aNode){
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);

  SoSearchAction saT;
  saT.setFind(SoSearchAction::TYPE);
  saT.setType(SoSceneGraph::getClassTypeId());
  //saT.setSearchingAll(TRUE); //to search all under an SoSwitch.
  saT.setInterest(SoSearchAction::ALL);
  saT.apply(aNode);
  SoPathList& plT = saT.getPaths();

  SoSearchAction saI;
  saI.setFind(SoSearchAction::TYPE);
  saI.setType(SoInfo::getClassTypeId());
  //saI.setSearchingAll(TRUE); //to search all under an SoSwitch.
  saI.setInterest(SoSearchAction::ALL);
  saI.apply(aNode);
  SoPathList& plI = saI.getPaths();

  SoSearchAction saN;
  saN.setFind(SoSearchAction::NAME);
  saN.setName("sceneGraph"); //DEPRECATED : old logic.
  //saN.setSearchingAll(TRUE); //to search all under an SoSwitch.
  saN.setInterest(SoSearchAction::ALL);
  saN.apply(aNode);
  SoPathList& plN = saN.getPaths();

  SoBaseKit::setSearchingChildren(oldsearch);

  SoPathList* list = new SoPathList(plT);
  if(!list) return 0;

 {unsigned int number = plI.getLength();
  for(unsigned int index=0;index<number;index++) {
    SoPath* path = plI[index];
    SoInfo* info = (SoInfo*)(path->getTail());
    const SbString& s = info->string.getValue();
    if((s.getLength()>=5)&&(s.getSubString(0,4)=="name:")) { 
      list->append(path);
    }
  }}

 {unsigned int number = plN.getLength();
  for(unsigned int index=0;index<number;index++) {
    SoFullPath* path = (SoFullPath*)plN[index];
    list->append(path);
  }}

  return list;
}
SoNode* SoTools::searchNode(SoNode* aNode,SoType aType){
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction searchAction;
  SoNode* node = 0;
  searchAction.setFind(SoSearchAction::TYPE);
  searchAction.setType(aType,TRUE);
  searchAction.apply(aNode);
  searchAction.setInterest(SoSearchAction::FIRST);
  SoBaseKit::setSearchingChildren(oldsearch);
  SoFullPath* path = (SoFullPath*)searchAction.getPath();
  if(path) node = path->getTail();
  if(node && (node->isOfType(aType)==FALSE)) { //Check
    SoDebugError::postInfo("SoTools::searchNode",
      "Get type %s whilst %s was expected !",
      node->getTypeId().getName().getString(),aType.getName().getString());
    return 0;
  }
  return node;
}

SoNode* SoTools::searchNode(SoNode* aNode,const SbString& aName){
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction searchAction;
  SoNode* node = 0;
  searchAction.setFind(SoSearchAction::NAME);
  searchAction.setName(aName.getString());
  searchAction.apply(aNode);
  SoBaseKit::setSearchingChildren(oldsearch);
  SoFullPath* path = (SoFullPath*)searchAction.getPath();
  if(path) node = path->getTail();
  if(node && 
      (::strcmp(node->getName().getString(),aName.getString())) 
    ){ //Check
    SoDebugError::postInfo("SoTools::searchNode",
      "Get node with name %s whilst %s was expected !",
      node->getName().getString(),aName.getString());
    return 0;
  }
  return node;
}

SoNode* SoTools::searchNode(SoNode* aNode,const SbString& aName,SoType aType){
  SoNode* soNode = searchNode(aNode,aName);
  if(soNode==NULL) return 0;
  if(soNode->isOfType(aType)==FALSE) return 0;
  return soNode;
}

SbBool SoTools::isChildOf(SoNode* aNode,SoNode* aQuery){
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction searchAction;
  searchAction.setFind(SoSearchAction::NODE);
  searchAction.setNode(aQuery);
  searchAction.apply(aNode);
  SoBaseKit::setSearchingChildren(oldsearch);
  SoFullPath* path = (SoFullPath*)searchAction.getPath();
  SoNode* node = 0;
  if(path) node = path->getTail();
  return node?TRUE:FALSE;
}

void SoTools::addPolygonToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
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

#define LIST_SET(list,number,x,y,z) list[number].setValue((x),(y),(z))

void SoTools::addBoxCoords(SoSeparator& aSeparator,float a_w,float a_h,float a_d){
  SbVec3f points[24];
  SbVec3f normals[6];

  float dw =  a_w*0.5F;
  float dh =  a_h*0.5F;
  float dd =  a_d*0.5F;

  //bottom-y :
 {float yy = -a_h*0.5F;
  LIST_SET(points,0,-dw, yy, dd);
  LIST_SET(points,1,-dw, yy,-dd);
  LIST_SET(points,2, dw, yy,-dd);
  LIST_SET(points,3, dw, yy, dd);
  LIST_SET(normals,0,0,-1,0);}

  //top-y :
 {float yy =  a_h*0.5F;
  LIST_SET(points,4,-dw, yy, dd);
  LIST_SET(points,5, dw, yy, dd);
  LIST_SET(points,6, dw, yy,-dd);
  LIST_SET(points,7,-dw, yy,-dd);
  LIST_SET(normals,1,0,1,0);}

  //front-x :
 {float xx =  a_w*0.5F;
  LIST_SET(points, 8,xx,-dh,-dd);
  LIST_SET(points, 9,xx, dh,-dd);
  LIST_SET(points,10,xx, dh, dd);
  LIST_SET(points,11,xx,-dh, dd);
  LIST_SET(normals,2,1,0,0);}

  //back-x :
 {float xx = -a_w*0.5F;
  LIST_SET(points,12,xx,-dh,-dd);
  LIST_SET(points,13,xx,-dh, dd);
  LIST_SET(points,14,xx, dh, dd);
  LIST_SET(points,15,xx, dh,-dd);
  LIST_SET(normals,3,-1,0,0);}

  //back-z :
 {float zz = -a_d*0.5F;
  LIST_SET(points,16,-dw,-dh, zz);
  LIST_SET(points,17,-dw, dh, zz);
  LIST_SET(points,18, dw, dh, zz);
  LIST_SET(points,19, dw,-dh, zz);
  LIST_SET(normals,4,0,0,-1);}

  //front-z :
 {float zz =  a_d*0.5F;
  LIST_SET(points,20,-dw,-dh, zz);
  LIST_SET(points,21, dw,-dh, zz);
  LIST_SET(points,22, dw, dh, zz);
  LIST_SET(points,23,-dw, dh, zz);
  LIST_SET(normals,5,0,0,1);}

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = 24;
  coordinate3->point.setValues(0,pointn,points);
  aSeparator.addChild(coordinate3);

  SoNormal* normal = new SoNormal;
  normal->vector.setValues(0,6,normals);
  aSeparator.addChild(normal);
}

void SoTools::addPolygonToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine,const SbVec3f& aNormal) {
  if(aNumber<=0) return;

  SoNormal* normal = new SoNormal;
  normal->vector.setValues(0,1,&aNormal);
  aSeparator->addChild(normal);

  addPolygonToNode(aSeparator,aNumber,aLine);
}

void SoTools::addLinesToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=0) return;
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,&pointn);
  aSeparator->addChild(lineSet);
}

void SoTools::addSegmentsToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
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

void SoTools::addNurbsCurveToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return; //Need at least two points.

  SbVec3f* tangents = new SbVec3f[aNumber];
  tangents[0] = aLine[1] - aLine[0];
  for(int i = 1;i<(aNumber-1);i++) tangents[i] = aLine[i+1] - aLine[i-1];
  tangents[aNumber-1] = aLine[aNumber-1] - aLine[aNumber-2];

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  aSeparator->addChild(coordinate3);

  float knots[8] = {0,0,0,0,1,1,1,1};

  SbVec3f points[4];
  int32_t pointn = 4;
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

    coordinate3->point.setValues(icoord,pointn,points);
    icoord += pointn;

    SoIndexedNurbsCurve* nurbsCurve = new SoIndexedNurbsCurve;
    nurbsCurve->numControlPoints.setValue(4);
    nurbsCurve->knotVector.setValues(0,8,knots);
    nurbsCurve->coordIndex.setValues(0,pointn+1,coordIndex);
    aSeparator->addChild(nurbsCurve);

  }

  delete [] tangents;
}

void SoTools::addSplineCurveToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return; //Need at least two points.

  printf("debug : SoTools::addSplineCurveToNode : %d\n",aNumber);

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  SbVec3f* tangents = new SbVec3f[aNumber];
  tangents[0] = aLine[1] - aLine[0];
  for(int i = 1;i<(aNumber-1);i++) tangents[i] = aLine[i+1] - aLine[i-1];
  tangents[aNumber-1] = aLine[aNumber-1] - aLine[aNumber-2];

  SoSplineCurve* splineCurve = new SoSplineCurve;
  splineCurve->points.setValues(0,aNumber,aLine);
  splineCurve->tangents.setValues(0,aNumber,tangents);
  splineCurve->generateAlternateRep(0);
  aSeparator->addChild(splineCurve);

  delete [] tangents;
}

class SbLagrangePolynomial {
public:
  inline SbLagrangePolynomial(int aNumber,float* aXs,float* aYs)
  :fNumber(0)
  ,fXs(0)
  ,fYs(0)
  ,fDenominators(0)
  {
    // aYs and aXs must be of size aNumber.
    // WARNING : All the aXs must have a different value !
    if(aNumber<=0) return;

    // Check aXs :
   {for(int i = 0;i<aNumber;i++) {
      //printf("debug : %d (%d) : %g %g\n",i,aNumber,aXs[i],aYs[i]);
      for(int j = i+1;j<aNumber;j++) {
        if(aXs[j]==aXs[i]) {
          SoDebugError::postInfo("SbLagrangePolynomial::SbLagrangePolynomial",
          "%d and %d points have same X (%g %g) (%g %g)",
          i,j,aXs[i],aYs[i],aXs[j],aYs[j]);
          return;
        }
      }
    }}
  
    fXs = new float[aNumber];
    fYs = new float[aNumber];
    for(int i = 0;i<aNumber;i++) {
      fXs[i] = aXs[i];
      fYs[i] = aYs[i];
    }
  
    // Initialize :
    fDenominators = new float[aNumber];
   {for(int i = 0;i<aNumber;i++) {
      fDenominators[i] = 1;
      for(int j = 0;j<aNumber;j++) {
        if(j==i) continue;
        fDenominators[i] *= (aXs[i]-aXs[j]);
      }
    }}
  
    fNumber = aNumber;
  }

  virtual ~SbLagrangePolynomial(){
    delete [] fDenominators;
    delete [] fXs;
    delete [] fYs;
  }
public:
  inline SbBool isValid() const {return (fNumber<=0?FALSE:TRUE);}

  inline float getValue(float aX) const {
    if(fNumber<=0) return 0;
    float Y = 0;
    for(int i = 0;i<fNumber;i++) {
      float numerator = 1;
      for(int j = 0;j<fNumber;j++) {
        if(j==i) continue;
        numerator *= (aX - fXs[j]);
      }
      Y += fYs[i] * numerator/fDenominators[i];
    }
    return Y;
  }

private:
  int fNumber;
  float* fXs;
  float* fYs;
  float* fDenominators;
};

void SoTools::addLagrangePolynomialCurveToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return; //Need at least two points.

  float* xs = new float[aNumber];
  float* ys = new float[aNumber];
  float xmn = aLine[0][0];
  float xmx = xmn;
 {for(int i = 0;i<aNumber;i++) {
    xs[i] = aLine[i][0];
    ys[i] = aLine[i][1];
    xmn = SbMinimum(xmn,xs[i]);
    xmx = SbMaximum(xmx,xs[i]);
  }}
  SbLagrangePolynomial lp(aNumber,xs,ys);
  delete [] xs;
  delete [] ys;

  if(lp.isValid()==FALSE) {
    SoDebugError::postInfo("SoTools::addCurveToNode",
      "Bad input ; some points have same x.");
    return;
  }

  int nseg = aNumber * 100;
  float xrange = (xmx - xmn)/nseg;
  SbVec3f* points = new SbVec3f[nseg+1];
 {for(int i = 0;i<=nseg;i++) {
    float x = xmn + i * xrange;
    points[i].setValue(x,lp.getValue(x),0);
    //printf("debug : SoTools::addCurveToNode : %d (%d) : %g %g\n",
    //  i,nseg,x,lp.getValue(x));
  }}  

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = nseg+1;
  coordinate3->point.setValues(0,pointn,points);
  aSeparator->addChild(coordinate3);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,&pointn);
  aSeparator->addChild(lineSet);

  delete [] points;
}

void SoTools::addPointsToNode (
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

void SoTools::addAlignedStringToNode (
 SoSeparator* aSeparator 
,const SbString& aFont
,const SbString& aEncoding
,SbBool aSmoothing
,const SbString& aString  
,const SbVec3f& aVertex
,const SbVec3f& aX
,const SbVec3f& aY
,float aSize    
,HorizontalJustification aHJust
,VerticalJustification aVJust
,SbBool aRotated
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

  } else if(SbStringIsTTF(aFont)==TRUE) {
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
    text->size.setValue(SbFROUND(aSize * majic)); // around 10
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbBool SoTools::setKitPart(SoBaseKit* aKit,const char* aString,const char* aValue){
  // If aString is a SoField then we do a 
  //   node.set("<field> <value>")
  // If aString is a "part.field" then we do a 
  //   node.set("<part>","<field> <value>")

  //printf("debug : SoTools::setKitPart : |%s|%s|\n",aString,aValue);

  char* dot = ::strrchr((char*)aString,'.');

  SoField* soField = 0;
  if(dot==NULL) {
    soField = aKit->getField(aString); //it creates an SbName.
    if(!soField) {
      SoDebugError::postInfo("SoTools::setKitPart",
                             "Unable to find SoField \"%s\".",aString);
      return FALSE;
    }
  } else {
    *dot = 0;
    SoNode* node = aKit->getPart(aString,TRUE);
    //WARNING : SoBaseKit::set seems bugged.
    if(!node) {
      SoDebugError::postInfo("SoTools::setKitPart",
                             "Unable to get part \"%s\".",
                             aString);
      *dot = '.';
      return FALSE;
    }
    *dot = '.';
    soField = node->getField(dot+1);
    if(!soField) {
      SoDebugError::postInfo("SoTools::setKitPart",
                             "Unable to find SoField \"%s\".",
                             dot+1);
      return FALSE;
    }
  }

  SbColor sbColor;
  if(soField->isOfType(SoSFString::getClassTypeId())) {
    ((SoSFString*)soField)->setValue(aValue);
  } else if( soField->isOfType(SoSFColor::getClassTypeId()) ) {
    if(SbStyle::stringToColor(aValue,sbColor)==TRUE) {
      ((SoSFColor*)soField)->setValue(sbColor);
    } else {
      SoDebugError::postInfo("SoTools::setKitPart",
                             "string color \"%s\" not found.",aValue);
      return FALSE;
    }
  } else { 
    if(soField->set(aValue)==FALSE) {
      SoDebugError::postInfo("SoTools::setKitPart",
                             "Unable to set \"%s\" with value \"%s\".",
                             aString,aValue);
      return FALSE;
    }
  }
  return TRUE;
}

SbBool SoTools::getKitPart(SoBaseKit* aKit,const char* aString,SbString& aValue){
  // aString = field then do a get("<field>")
  // aString = part.field then do a getPart("<part>") and then
  //  do a <field>.get("<value>")

  //printf("debug : SoTools::getKitPart :%s|...\n",aString);

  char* dot = ::strrchr((char*)aString,'.');

  SoField* soField = 0;
  if(dot==NULL) {
    soField = aKit->getField(aString);
    if(!soField) {
      SoDebugError::postInfo("SoTools::getKitPart",
                             "Unable to find SoField \"%s\".",aString);
      return FALSE;
    }
  } else {
    *dot = 0;
    SoNode* node = aKit->getPart(aString,FALSE);
    if(!node) {
      SoDebugError::postInfo("SoTools::getKitPart",
                             "Unable to get part \"%s\".",
                             aString);
      *dot = '.';
      return FALSE;
    }
    *dot = '.';
    soField = node->getField(dot+1);
    if(!soField) {
      SoDebugError::postInfo("SoTools::getKitPart",
                             "Unable to find SoField \"%s\".",
                             dot+1);
      return FALSE;
    }
  }
  if(soField->isOfType(SoSFString::getClassTypeId())) {
    aValue = ((SoSFString*)soField)->getValue();
  } else {
    soField->get(aValue);
  }
  return TRUE;
}

SbBool SoTools::toggleKitPart(SoBaseKit* aKit,const char* aString) {
  SbString svalue;
  if(getKitPart(aKit,aString,svalue)==FALSE) return FALSE;
  SbBool value;
  if(SbConvertToBool(svalue,value)==FALSE) {
    SoDebugError::postInfo("SoTools::toggleKitPart",
                           "\"%s\" not a boolean field.",aString);
    return FALSE;
  }  
  if(value) {
    return setKitPart(aKit,aString,"FALSE");
  } else {
    return setKitPart(aKit,aString,"TRUE");
  }  
}


//////////////////////////////////////////////////////////////////////////////
/// deprecated, use the uppers ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbBool SoTools::setKitPartValue(SoBaseKit* aKit,const SbString& aString,const SbString& aValue){
  // If aString is a SoField then we do a 
  //   node.set("<field> <value>")
  // If aString is a "part.field" then we do a 
  //   node.set("<part>","<field> <value>")

  //printf("debug : SoTools::setKitPartValue :%s|%s|\n",
  //  aString.getString(),aValue.getString());
  int dot = SbStringReverseFind(aString,".");
  SoField* soField = 0;
  SbString field;
  if(dot==SbStringNotFound) {
    field = aString;
    soField = aKit->getField(field);
  } else {
    SbString part = aString.getSubString(0,dot-1);
    //WARNING : SoBaseKit::set seems bugged.
    SoNode* node = aKit->getPart(part,TRUE);
    if(!node) {
      SoDebugError::postInfo("SoTools::setKitPartValue",
                             "Unable to get part \"%s\".",
                             part.getString());
      return FALSE;
    }
    field = aString.getSubString(dot+1,aString.getLength()-1);
    soField = node->getField(field);
  }
  if(!soField) {
    SoDebugError::postInfo("SoTools::setKitPartValue",
                           "Unable to find SoField \"%s\".",
                           field.getString());
    return FALSE;
  }

  SbColor sbColor;
  if(soField->isOfType(SoSFString::getClassTypeId())) {
    ((SoSFString*)soField)->setValue(aValue);
  } else if( soField->isOfType(SoSFColor::getClassTypeId()) &&
             (SbStyle::stringToColor(aValue,sbColor)==TRUE) ) {
    ((SoSFColor*)soField)->setValue(sbColor);
  } else { 
    if(soField->set(aValue.getString())==FALSE) {
      SoDebugError::postInfo("SoTools::setKitPartValue",
                             "Unable to set \"%s\" with value \"%s\".",
                             aString.getString(),aValue.getString());
      return FALSE;
    }
  }
  return TRUE;
}

SbBool SoTools::getKitPartValue(SoBaseKit* aKit,const SbString& aString,SbString& aValue){
  // aString = field then do a get("<field>")
  // aString = part.field then do a getPart("<part>") and then
  //  do a <field>.get("<value>")

  //printf("debug : SoTools::getKitPartValue :%s|...\n",
  //  aString.getString());
  int dot = SbStringReverseFind(aString,".");
  SoField* soField = 0;
  if(dot==SbStringNotFound) {
    soField = aKit->getField(aString);
    if(!soField) {
      SoDebugError::postInfo("SoTools::getKitPartValue",
                             "Unable to find SoField \"%s\".",
                             aString.getString());
      return FALSE;
    }
  } else {
    SbString part = aString.getSubString(0,dot-1);
    SbString field = aString.getSubString(dot+1,aString.getLength()-1);
    SoNode* node = aKit->getPart(part,FALSE);
    if(!node) {
      SoDebugError::postInfo("SoTools::getKitPartValue",
                             "Unable to get part \"%s\".",
                             part.getString());
      return FALSE;
    }
    soField = node->getField(field);
    if(!soField) {
      SoDebugError::postInfo("SoTools::getKitPartValue",
                             "Unable to find SoField \"%s\".",
                             field.getString());
      return FALSE;
    }
  }
  if(soField->isOfType(SoSFString::getClassTypeId())) {
    aValue = ((SoSFString*)soField)->getValue();
  } else {
    soField->get(aValue);
  }
  return TRUE;
}

SbBool SoTools::toggleKitPartValue(SoBaseKit* aKit,const SbString& aString) {
  //printf("debug : SoTools::toggleKitPartValue :%s|\n",aString.getString());
  SbString svalue;
  if(getKitPartValue(aKit,aString,svalue)==FALSE) return FALSE;
  SbBool value;
  if(SbConvertToBool(svalue,value)==FALSE) {
    SoDebugError::postInfo("SoTools::toggleKitPartValue",
                           "\"%s\" not a boolean field.",aString.getString());
    return FALSE;
  }  
  if(value) {
    return setKitPartValue(aKit,aString,"FALSE");
  } else {
    return setKitPartValue(aKit,aString,"TRUE");
  }  
}

const SbName& SoTools::highlightName() {
  static const SbName name("highlight"); //read only static, then ok.
  return name;
}

void SoTools::highlight(SoFullPath& aPath){
  SoNode* node = aPath.getTail();
  if(!node) return;

  //printf("debug : SoTools::highlight: pick : \"%s\" \"%s\"\n",
  // node->getName().getString(),
  // node->getTypeId().getName().getString());
  
  //Note : the SoMaterial is not a parent and then not in the path...

  // Search a material node :
  int l = aPath.getLength();
  for(int index=1;index<l;index++) {
    SoNode* n = aPath.getNodeFromTail(index);
    /*
      printf("debug : pick path : %d : name \"%s\" type \"%s\"\n",
      index,
      n->getName().getString(),
      n->getTypeId().getName().getString());
    */
    //if(n->getName()==name) {
    if(n->isOfType(SoGroup::getClassTypeId())) {
      SoSearchAction action;
      action.setFind(SoSearchAction::TYPE);
      action.setType(SoMaterial::getClassTypeId());
      action.apply(n);
      SoPath* path = action.getPath();
      if(path) {
        SoMaterial* saveMaterial = (SoMaterial*)path->getTail();
        if(path->getLength()>=2) {
          SoNode* parent = path->getNodeFromTail(1);
          if(parent->isOfType(SoGroup::getClassTypeId())) {

            if(saveMaterial->isOfType
               (SoHighlightMaterial::getClassTypeId())==TRUE) {
              if(((SoHighlightMaterial*)saveMaterial)->getSavedMaterial())
                break; // Already highlighted.
            }

            // Get highlight color :
            SbColor hcolor(1,0,0);
            if(saveMaterial->isOfType
               (SoHighlightMaterial::getClassTypeId())==TRUE) {
              hcolor = ((SoHighlightMaterial*)saveMaterial)->highlightColor[0];
            }
            SbColor dcolor = saveMaterial->diffuseColor[0]; 
            if(hcolor==dcolor) {
              float h,s,v;
              dcolor.getHSVValue(h,s,v);
              hcolor.setHSVValue(h,s,v==1?0.6f:1);
            }

            SoHighlightMaterial* material = new SoHighlightMaterial;
            material->setName(SoTools::highlightName());
            material->setSavedMaterial(saveMaterial);
              material->diffuseColor.setValue(hcolor); // highlight color.
            material->ambientColor = saveMaterial->ambientColor;
            //material->diffuseColor = saveMaterial->diffuseColor;
            material->specularColor = saveMaterial->specularColor;
            material->emissiveColor = saveMaterial->emissiveColor;
            material->shininess = saveMaterial->shininess;
            material->transparency = saveMaterial->transparency;

            saveMaterial->ref();
            ((SoGroup*)parent)->replaceChild(saveMaterial,material);
            //material had been referenced.

            break;
          }

        }
      }
    }
  }
}
/*
void SoTools::dumpPath(SoFullPath& aPath){
  int l = aPath.getLength();
  ::printf("SoTools::dumpPath : length : %d\n",l);
  for(int index=0;index<l;index++) {
    SoNode* n = aPath.getNodeFromTail(index);
    ::printf(" index : %d : name \"%s\" type \"%s\"\n",
      index,
      n->getName().getString(),
      n->getTypeId().getName().getString());
  }
}
*/

void SoTools::resetHighlight(SoNode* aNode){
  // Needed to search in nodekits (for exa SoDetectorTreeKit) :
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction action;
  action.setFind(SoSearchAction::TYPE);
  action.setType(SoHighlightMaterial::getClassTypeId());
  action.setInterest(SoSearchAction::ALL);
  action.apply(aNode);
  SoBaseKit::setSearchingChildren(oldsearch);
  
  SoPathList& pathList = action.getPaths();
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoHighlightMaterial* material = (SoHighlightMaterial*)path->getTail();
    if(path->getLength()>=2) {
      SoNode* parent = path->getNodeFromTail(1);
      if(parent->isOfType(SoGroup::getClassTypeId())) {
        SoMaterial* savedMaterial = material->getSavedMaterial();
        if(savedMaterial) {
          ((SoGroup*)parent)->replaceChild(material,savedMaterial);
          //material dereferenced by the upper ; then deleted.
          //Restore "before highlight" ref count.
          savedMaterial->unref(); 
        } 
      }
    }
  }
}

void SoTools::pointAt(SoCamera& aCamera,const SbVec3f& aTarget,const SbVec3f& aUp)
{
  // Taken from Coin3d code.
  SbVec3f dir = aTarget - aCamera.position.getValue();
  if(dir.normalize()==0.0F) return;
  SoTools::lookAt(aCamera,dir,aUp);
}

void SoTools::lookAt(SoCamera& aCamera,const SbVec3f& aDir,const SbVec3f& aUp){
  // Taken from Coin3d code.
  SbVec3f z = -aDir;
  SbVec3f y = aUp;
  SbVec3f x = y.cross(z);

  // recompute y to create a valid coordinate system
  y = z.cross(x);

  // normalize x and y to create an orthonormal coord system
  y.normalize();
  x.normalize();

  // create a rotation matrix
  SbMatrix rot = SbMatrix::identity();
  rot[0][0] = x[0];
  rot[0][1] = x[1];
  rot[0][2] = x[2];

  rot[1][0] = y[0];
  rot[1][1] = y[1];
  rot[1][2] = y[2];

  rot[2][0] = z[0];
  rot[2][1] = z[1];
  rot[2][2] = z[2];

  aCamera.orientation.setValue(SbRotation(rot));
}

SbBool SoTools::write(
 SoNode& aNode
,const SbString& aFile
,const SbString& aFormat
,SbBool aBinary
,SbBool aGenAlternateRep
) {
  //old API without the compression argument.
  return write(aNode,aFile,aFormat,aGenAlternateRep,aBinary,"",0.5F);
}

SbBool SoTools::write(
 SoNode& aNode
,const SbString& aFile
,const SbString& aFormat
,SbBool aGenAlternateRep
,SbBool aBinary
,const SbString& aCompresser
,float aCompressionLevel
){
  SbBool status = TRUE;

  SbString format = (aFormat==""?SbStringSuffix(aFile):aFormat);

  SbBool altRep = aGenAlternateRep;
  // Enforce altRep for some formats :
  SoAlternateRepAction alternateRepAction;
  if( (format=="iv") || (format=="inventor") )  {
    altRep = TRUE;
  } else if( (format=="wrl") || (format=="vrml2") ) {
    altRep = TRUE;
    alternateRepAction.setForWhat(SoAlternateRepAction::VRML);
  }

  if(altRep==TRUE) {
    alternateRepAction.setGenerate(TRUE); //Generate alternate reps.
    alternateRepAction.apply(&aNode);
  }

  if( (format=="hiv") || (format=="iv") || (format=="inventor") )  {

    SoOutput out;
    if(aCompresser.getLength()) {
      unsigned int num;
      const SbName* ls = SoOutput::getAvailableCompressionMethods(num);
      for(unsigned int index=0;index<num;index++) {
        if(!::strcmp(ls[index].getString(),aCompresser.getString())) {
          out.setCompression(ls[index],aCompressionLevel);
        }
      }
    }
    out.openFile(aFile.getString());
    out.setBinary(aBinary);
    SoWriteAction writeAction(&out);
    writeAction.apply(&aNode);
    out.closeFile();
  
  } else if( (format=="wrl") || (format=="vrml2") ) {

#ifdef HEPVIS_HAS_VRML2

    SoToVRML2Action action;
    action.apply(&aNode); //Internally it uses a SoCallbackAction.
  
    SoVRMLGroup* vrmlRoot = action.getVRML2SceneGraph();
    vrmlRoot->ref();
  
   {SoOutput out;
    if(aCompresser.getLength()) {
      unsigned int num;
      const SbName* ls = SoOutput::getAvailableCompressionMethods(num);
      for(unsigned int index=0;index<num;index++) {
        if(!::strcmp(ls[index].getString(),aCompresser.getString())) {
          out.setCompression(ls[index],aCompressionLevel);
        }
      }
    }
    out.openFile(aFile.getString());
    out.setBinary(aBinary);
    out.setHeaderString("#VRML V2.0 utf8");
    SoWriteAction action(&out);
    action.apply(vrmlRoot);
    out.closeFile();}

    vrmlRoot->unref();

#else
    SoDebugError::postInfo("SoTools::write",
      "HEPVis not reconstructed with the cpp macro HEPVIS_HAS_VRML2.");
    status = FALSE;
#endif

  } else if(format=="pov") {

    if(aNode.isOfType(SoGroup::getClassTypeId())==FALSE) {
      SoDebugError::postInfo("SoTools::write",
        "Given node is not an SoSeparator.");
      status = FALSE;
    } else {
      //NOTE : no need to produce alternate reps since 
      //       iv2pov works on shape triangles.
      HEPVis_iv2pov((SoGroup*)&aNode,FALSE,FALSE,aFile);
    }

  } else {

    SoDebugError::postInfo("SoTools::write",
      "unknown format %s.",format.getString());
    status = FALSE;

  }

  if(altRep==TRUE) {
    alternateRepAction.setGenerate(FALSE); //Clear alternate reps.
    alternateRepAction.apply(&aNode);
  }

  return status;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SbBool SoTools::setManip(SoNode* aScene,SoTransform* aTransform) {
  if(aTransform->isOfType(SoTransformManip::getClassTypeId())==TRUE) 
    return TRUE; //done

  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction searchAction;
  searchAction.setFind(SoSearchAction::NODE);
  searchAction.setNode(aTransform);
  searchAction.setInterest(SoSearchAction::ALL);
  searchAction.apply(aScene);
  SoBaseKit::setSearchingChildren(oldsearch);
  SoPathList pathList = searchAction.getPaths();
  int number = pathList.getLength();
  if(number<=0) return FALSE;
  SoFullPath* path = (SoFullPath*)pathList[0];
  SoNode* node = path->getTail();
  if(node!=aTransform) return FALSE;
  SoTransformBoxManip* manip = new SoTransformBoxManip;
  manip->ref();
  return manip->replaceNode(path);
}

SbBool SoTools::visit(SoNode& aNode,SoType aWhat,SbVisitor& aVisitor){
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction action;
  action.setFind(SoSearchAction::TYPE);
  action.setType(aWhat);
  action.setInterest(SoSearchAction::ALL);
  action.apply(&aNode);
  SoBaseKit::setSearchingChildren(oldsearch);
  
  SoPathList& pathList = action.getPaths();
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];    
    if(aVisitor.visit(*path)==FALSE) return FALSE;
  }
  return TRUE;
}

class SoTools_rm_manip : public virtual SbVisitor {
public:
  virtual ~SoTools_rm_manip(){}
public:
  virtual SbBool visit(SoFullPath& aPath) {
    SoNode* node = aPath.getTail();
    if(!node) return TRUE;
    if(node->isOfType(SoTransformManip::getClassTypeId())==FALSE) return TRUE;
    SoTransformManip* manip = (SoTransformManip*)node;

    //SoTools::dumpPath(aPath);

    //WARNING : problem with the Coin-2 :
    //            manip->replaceManip(&aPath,NULL);
    //          if the manip is under a [preview,full]Separator of a
    //          SoDetectorTreeKit. If so we extract the path up to
    //          the first up SoGroup.

   {SoPath* path = &aPath;
    if(path->getTail()->isOfType(SoBaseKit::getClassTypeId())) {
      int l = aPath.getLength();
      for(int i=0;i<l;i++) {
        SoNode* n = aPath.getNodeFromTail(i);
        if(n->isOfType(SoGroup::getClassTypeId())==TRUE) {
          SoPath* p = aPath.copy(l-1-i);
          p->ref();
          manip->replaceManip(p,NULL);
          manip->unref();
          p->unref();
          return TRUE;
        }
      }
    }}

    manip->replaceManip(&aPath,NULL);
    manip->unref();
    return TRUE;
  }
};
void SoTools::removeManips(SoNode& aScene) {
  SoTools_rm_manip v;
  visit(aScene,SoTransformManip::getClassTypeId(),v);
}

class SoTools_set_manip : public virtual SbVisitor {
public:
  virtual ~SoTools_set_manip(){}
public:
  virtual SbBool visit(SoFullPath& aPath) {
    SoNode* node = aPath.getTail();
    if(!node) return TRUE;
    if(node->isOfType(SoTransformManip::getClassTypeId())==TRUE) return TRUE;
    SoTransformBoxManip* manip = new SoTransformBoxManip;
    manip->ref();
    manip->replaceNode(&aPath);
    return TRUE;
  }
};
void SoTools::setManips(SoNode& aScene) {
  SoTools_set_manip v;
  visit(aScene,SoTransform::getClassTypeId(),v);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/draggers/SoTranslate2Dragger.h>

SbBool SoTools::setDraggerColor(SoTranslate2Dragger& aDragger,const SbColor& aColor){
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

#include <HEPVis/draggers/SoDragPoint2D.h>

void SoTools::addDraggersToPolygon(SoSeparator& aSeparator,SoCoordinate3& aCoord,const SbColor& aColor,float aScale,SoDraggerCB* aFinishCB,void* aFinishData){

  //NOTE : it is assumed that aCoord contains a closed polyline.
  int num = aCoord.point.getNum();
  const SbVec3f* pts = aCoord.point.getValues(0);

  for(int index=0;index<(num-1);index++) {
    const SbVec3f& pt = pts[index];

    SoSeparator* sep = new SoSeparator;
    aSeparator.addChild(sep);

    SoTransform* tsf = new SoTransform();
    tsf->translation.setValue(pt);
    tsf->scaleFactor.setValue(aScale,aScale,aScale);
    sep->addChild(tsf);

    SoDragPoint2D* dragger = new SoDragPoint2D(aCoord,index,aScale);
    setDraggerColor(*dragger,aColor);
    if(aFinishCB) dragger->addFinishCallback(aFinishCB,aFinishData);
    sep->addChild(dragger);
  }
}

//////////////////////////////////////////////////////////////////////////////
/// intersection detection ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <iostream> //FIXME

static void intersec_highlight(SoNode* sceneGraphNode,const SbColor& color) {
  // It assumes a OnX-like scene graph structure.
  SoSearchAction searchAction;
  searchAction.setFind(SoSearchAction::TYPE);
  searchAction.setType(SoMaterial::getClassTypeId());
  searchAction.apply(sceneGraphNode);
  SoPath* path = searchAction.getPath();
  if(path && (path->getLength()>=2)) {
    SoNode* parent = path->getNodeFromTail(1);
    if(parent && parent->isOfType(SoGroup::getClassTypeId())) {
      // replace the SoMaterial that may come from a style cache.
      SoMaterial* old_mat = (SoMaterial*)path->getTail();
      SoMaterial* new_mat = new SoMaterial;
      new_mat->diffuseColor.setValue(color);
      ((SoGroup*)parent)->replaceChild(old_mat,new_mat);
    }
  }
}

static void intersec_manip(SoNode* sceneGraphNode) {
  // It assumes a OnX-like scene graph structure.
  SoSearchAction searchAction;
  searchAction.setFind(SoSearchAction::TYPE);
  searchAction.setType(SoTransform::getClassTypeId());
  searchAction.apply(sceneGraphNode);
  SoPath* path = searchAction.getPath();
  if(path) {
    SoNode* node = path->getTail();
    if(node->isOfType(SoTransform::getClassTypeId())) { 
      SoTransformManip* manip = new SoTransformBoxManip;
      if(manip) {
        manip->ref();
        manip->replaceNode(path);
      }
    }
  }
}

//#include <Inventor/collision/SoIntersectionDetectionAction.h>
// Use the "double" version :
#include <HEPVis/actions/SoIntersectionDetectionAction.h>
#define SoIntersectionDetectionAction HEPVis_SoIntersectionDetectionAction
#define SoIntersectingPrimitive HEPVis_SoIntersectingPrimitive

static SoIntersectionDetectionAction::Resp intersectionCB(
 void* aTag
,const SoIntersectingPrimitive* a1
,const SoIntersectingPrimitive* a2
) {
  SbBool stopAtFirst = *((SbBool*)aTag);

  std::cout << "intersection :" << std::endl;
 {const SoIntersectingPrimitive* ip = a1;
  if(ip->type==SoIntersectingPrimitive::TRIANGLE) {

    ::printf(" p1 triangle : high prec :\n");
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[0][0],ip->xf_vertex[0][1],ip->xf_vertex[0][2]);
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[1][0],ip->xf_vertex[1][1],ip->xf_vertex[1][2]);
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[2][0],ip->xf_vertex[2][1],ip->xf_vertex[2][2]);

    std::cout << " p1 triangle :" << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[0][0]
              << " " << ip->xf_vertex[0][1]
              << " " << ip->xf_vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[1][0]
              << " " << ip->xf_vertex[1][1]
              << " " << ip->xf_vertex[1][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[2][0]
              << " " << ip->xf_vertex[2][1]
              << " " << ip->xf_vertex[2][2] << std::endl;

    std::cout << " p1 triangle not placed :" << std::endl;
    std::cout << " "
              << " " << ip->vertex[0][0]
              << " " << ip->vertex[0][1]
              << " " << ip->vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[1][0]
              << " " << ip->vertex[1][1]
              << " " << ip->vertex[1][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[2][0]
              << " " << ip->vertex[2][1]
              << " " << ip->vertex[2][2] << std::endl;

  } else if(ip->type==SoIntersectingPrimitive::SEGMENT) {
    std::cout << " p1 segment :" << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[0][0]
              << " " << ip->xf_vertex[0][1]
              << " " << ip->xf_vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[1][0]
              << " " << ip->xf_vertex[1][1]
              << " " << ip->xf_vertex[1][2] << std::endl;

    std::cout << " p1 segment not placed :" << std::endl;
    std::cout << " "
              << " " << ip->vertex[0][0]
              << " " << ip->vertex[0][1]
              << " " << ip->vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[1][0]
              << " " << ip->vertex[1][1]
              << " " << ip->vertex[1][2] << std::endl;
  }}

 {const SoIntersectingPrimitive* ip = a2;
  if(ip->type==SoIntersectingPrimitive::TRIANGLE) {
    ::printf(" p2 triangle : high prec :\n");
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[0][0],ip->xf_vertex[0][1],ip->xf_vertex[0][2]);
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[1][0],ip->xf_vertex[1][1],ip->xf_vertex[1][2]);
    ::printf("  %.25g %.25g %.25g\n",
       ip->xf_vertex[2][0],ip->xf_vertex[2][1],ip->xf_vertex[2][2]);

    std::cout << " p2 triangle :" << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[0][0]
              << " " << ip->xf_vertex[0][1]
              << " " << ip->xf_vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[1][0]
              << " " << ip->xf_vertex[1][1]
              << " " << ip->xf_vertex[1][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[2][0]
              << " " << ip->xf_vertex[2][1]
              << " " << ip->xf_vertex[2][2] << std::endl;

    std::cout << " p2 triangle not placed :" << std::endl;
    std::cout << " "
              << " " << ip->vertex[0][0]
              << " " << ip->vertex[0][1]
              << " " << ip->vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[1][0]
              << " " << ip->vertex[1][1]
              << " " << ip->vertex[1][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[2][0]
              << " " << ip->vertex[2][1]
              << " " << ip->vertex[2][2] << std::endl;

  } else if(ip->type==SoIntersectingPrimitive::SEGMENT) {
    std::cout << " p2 segment :" << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[0][0]
              << " " << ip->xf_vertex[0][1]
              << " " << ip->xf_vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->xf_vertex[1][0]
              << " " << ip->xf_vertex[1][1]
              << " " << ip->xf_vertex[1][2] << std::endl;

    std::cout << " p2 segment not placed :" << std::endl;
    std::cout << " "
              << " " << ip->vertex[0][0]
              << " " << ip->vertex[0][1]
              << " " << ip->vertex[0][2] << std::endl;
    std::cout << " "
              << " " << ip->vertex[1][0]
              << " " << ip->vertex[1][1]
              << " " << ip->vertex[1][2] << std::endl;
  }}

 {std::cout << " p1 path :" << std::endl;
  SoFullPath* path = (SoFullPath*)a1->path;
  if(path) {
    for(int i=0;i<path->getLength();i++) {
      SoNode* n = path->getNodeFromTail(i);
      if(n->isOfType(SoShape::getClassTypeId()))  {
        std::cout << "  type : " << n->getTypeId().getName().getString()
                   << std::endl;
        SbName name = n->getName();
        if(name.getLength()) {
          std::cout << "  name : \"" << name.getString() << "\"" << std::endl;
        }
        break;
      }
    }
  }}

 {std::cout << " p2 path :" << std::endl;
  SoFullPath* path = (SoFullPath*)a2->path;
  if(path) {
    for(int i=0;i<path->getLength();i++) {
      SoNode* n = path->getNodeFromTail(i);
      if(n->isOfType(SoShape::getClassTypeId()))  {
        std::cout << "  type : " << n->getTypeId().getName().getString() 
                   << std::endl;
        SbName name = n->getName();
        if(name.getLength()) {
          std::cout << "  name : \"" << name.getString() << "\"" << std::endl;
        }
        break;
      }
    }
  }}


  // look if shape is under an "OnX kind scene graph: :
 {SoFullPath* path = (SoFullPath*)a1->path;
  if(path) {
    for(int i=0;i<path->getLength();i++) {
      SoNode* n = path->getNodeFromTail(i);
      if( (n->isOfType(SoSceneGraph::getClassTypeId())==TRUE) ||
          (n->getName()=="sceneGraph") ){
        std::cout << " set p1 with a manip." << std::endl;
        //intersec_highlight(n,SbColor(0,0,1));
        intersec_manip(n);
        break;
      }
    }
  }}
 {SoFullPath* path = (SoFullPath*)a2->path;
  if(path) {
    for(int i=0;i<path->getLength();i++) {
      SoNode* n = path->getNodeFromTail(i);
      if( (n->isOfType(SoSceneGraph::getClassTypeId())==TRUE) ||
          (n->getName()=="sceneGraph") ){
        std::cout << " set p2 with a manip." << std::endl;
        //intersec_highlight(n,SbColor(0,1,0));
        intersec_manip(n);
        break;
      }
    }
  }}

  //return SoIntersectionDetectionAction::NEXT_PRIMITIVE;
  if(stopAtFirst==TRUE) {
    return SoIntersectionDetectionAction::ABORT;
  } else {
    return SoIntersectionDetectionAction::NEXT_SHAPE;
  }
}

SbBool SoTools::detectIntersection(SoNode& aNode,SbBool aStopAtFirst){
  SoIntersectionDetectionAction action;
  //printf("debug : the epsi %g\n",action.getIntersectionDetectionEpsilon());
  action.addIntersectionCallback(intersectionCB,&aStopAtFirst);
  action.apply(&aNode);
  return TRUE;
}

struct Counter {
 unsigned int fTriangles;
 unsigned int fLineSegments;
 unsigned int fPoints;
};

static void CountTrianglesCB(
 void* userData
,SoCallbackAction*
,const SoPrimitiveVertex*
,const SoPrimitiveVertex*,
const SoPrimitiveVertex*)
{
  Counter* counter = (Counter*)userData;
  counter->fTriangles++;
}

static void CountLineSegmentsCB(
 void* userData
,SoCallbackAction*
,const SoPrimitiveVertex*
,const SoPrimitiveVertex*)
{
  Counter* counter = (Counter*)userData;
  counter->fLineSegments++;
}

static void CountPointsCB(
 void* userData
,SoCallbackAction*
,const SoPrimitiveVertex*)
{
  Counter* counter = (Counter*)userData;
  counter->fPoints++;
}


void SoTools::count(
 SoNode& aNode
,unsigned int& aTriangles
,unsigned int& aLineSegments
,unsigned int& aPoints
,unsigned int& aNodes
,unsigned int& aShapes
){
  Counter counter;
  counter.fTriangles = 0;
  counter.fLineSegments = 0;
  counter.fPoints = 0;

  SoCallbackAction callbackAction;
  callbackAction.addTriangleCallback
    (SoShape::getClassTypeId(),CountTrianglesCB,(void*)&counter);
  callbackAction.addLineSegmentCallback
    (SoShape::getClassTypeId(),CountLineSegmentsCB,(void*)&counter);
  callbackAction.addPointCallback
    (SoShape::getClassTypeId(),CountPointsCB,(void*)&counter);
  callbackAction.apply(&aNode);

  SoCounterAction counterAction;
  counterAction.apply(&aNode);
  aNodes = counterAction.getCount();

  counterAction.setLookFor(SoCounterAction::TYPE);
  counterAction.setType(SoShape::getClassTypeId());
  counterAction.apply(&aNode);
  aShapes = counterAction.getCount();

  aTriangles = counter.fTriangles;
  aLineSegments = counter.fLineSegments;
  aPoints = counter.fPoints;
}
