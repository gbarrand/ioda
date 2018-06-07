// this :
#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCamera.h>

#include <HEPVis/misc/SoTools.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoViewportRegion.h>

SO_KIT_SOURCE(SoRegion) 

void SoRegion::initClass(){
  SO_KIT_INIT_CLASS(SoRegion,SoBaseKit,"BaseKit");
}

SoRegion::SoRegion()
:fUpdate(FALSE)
,fStyleCache(0)
,fPAW_Rows(-1)
,fPAW_Cols(-1)
,fPAW_Index(-1)
{
  SO_KIT_CONSTRUCTOR(SoRegion);
  SO_KIT_ADD_CATALOG_ENTRY(viewportRegion,SoViewportRegion,FALSE,this,\0,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(visible,(TRUE));
  SO_NODE_ADD_FIELD(verbose,(FALSE));

  SO_NODE_ADD_FIELD(origin,(SbVec2f(0,0)));
  SO_NODE_ADD_FIELD(size,(SbVec2f(1,1)));
  SO_NODE_ADD_FIELD(color,(SbColor(1,1,1)));

  SO_NODE_ADD_FIELD(horizontalMargin,(2));
  SO_NODE_ADD_FIELD(verticalMargin,(2));

  // begin constraint fields (not used by the nodekit itself) :
  SO_NODE_ADD_FIELD(horizontalJustification,(LEFT));
  SO_NODE_ADD_FIELD(verticalJustification,(BOTTOM));

  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,CENTER);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,RIGHT);

  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,BOTTOM);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,MIDDLE);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,TOP);

  SO_NODE_SET_SF_ENUM_TYPE(horizontalJustification,HorizontalJustification);
  SO_NODE_SET_SF_ENUM_TYPE(verticalJustification,VerticalJustification);

  SO_NODE_ADD_FIELD(originUnit,(PERCENT));
  SO_NODE_DEFINE_ENUM_VALUE(Unit,PERCENT);
  SO_NODE_DEFINE_ENUM_VALUE(Unit,AXIS);
  SO_NODE_SET_SF_ENUM_TYPE(originUnit,Unit);
  // end constraint fields.

  SoViewportRegion* soViewportRegion = (SoViewportRegion*)viewportRegion.getValue();
  soViewportRegion->backgroundColor.connectFrom(&color);
  soViewportRegion->origin.connectFrom(&origin);
  soViewportRegion->size.connectFrom(&size);

  fStyleCache = new SoStyleCache;
  fStyleCache->ref();
}

SoRegion::~SoRegion(){
  fStyleCache->unref();

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();
  soViewportRegion->backgroundColor.disconnect();
}

void SoRegion::GLRender(SoGLRenderAction* aAction){
  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoRegion::GLRender","%ld\n",this);
  }
  if(visible.getValue()==FALSE) return;
  SoBaseKit::GLRender(aAction);
}

SoNode* SoRegion::findPart(const SbName& aName){
  // getPart get only public leaf. We need more (get topSeparator).
  return this->getAnyPart(aName,FALSE,FALSE,FALSE);
}

SoNode* SoRegion::findNode(const SbString& aWhat,const SbString& aString){
  if(aWhat=="part") {
    SoNode* node = findPart(aString.getString());
    if(!node) {
      SoDebugError::postInfo("SoRegion::findNode",
        "Node with kit catalog entry name \"%s\" not found.",aString.getString());
    }
    return node;
  } else if(aWhat=="name") {
    SoNode* node = SoTools::searchNode(this,aString.getString());
    if(!node) {
      SoDebugError::postInfo("SoRegion::findNode",
        "Node with name \"%s\" not found.",aString.getString());
    }
    return node;
  } else if(aWhat=="type") {
    SoNode* node = 
      SoTools::searchNode(this,SoType::fromName(aString.getString()));
    if(!node) {
      SoDebugError::postInfo("SoRegion::findNode",
        "Node with type \"%s\" not found.",aString.getString());
    }
    return node;
  } else {
    SoDebugError::postInfo("SoRegion::findNode",
      "Unknow kind \"%s\".",aWhat.getString());
    return 0;
  }
}

SoViewportRegion* SoRegion::getViewportRegion() const{return (SoViewportRegion*)viewportRegion.getValue();}

SoStyleCache* SoRegion::styleCache() const{return fStyleCache;}

void SoRegion::clearStyleCache(){fStyleCache->clearCache();}

void SoRegion::pushCamera(){
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;
  fCameraStack.push_front(RegionCamera(*soCamera));
}

void SoRegion::popCamera(){
  if(fCameraStack.empty()) return;
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;
  RegionCamera& regionCamera = fCameraStack.front();
  regionCamera.restore(*soCamera);
  fCameraStack.pop_front();
}

void SoRegion::clearCameraStack(){fCameraStack.clear();}

void SoRegion::viewRectangle(
 const SbVec2s& aPos  // Rectangle position (GL coordinates).
,const SbVec2s& aSize // Rectangle size.
,const SbVec2s& aWindow
){
  if(aWindow[0]<=0) return;
  if(aWindow[1]<=0) return;
  if(aSize[0]<=0) return;
  if(aSize[1]<=0) return;
  
  //pushCamera();
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();

  int rx,ry;
  soViewportRegion->getPositionPixels(rx,ry);
  unsigned int rw,rh;
  soViewportRegion->getSizePixels(rw,rh);
  if(rw<=0) return;
  if(rh<=0) return;

  // Take middle of rectangle :
  float x = (aPos[0] + ((float)aSize[0])/2 - rx)/((float)rw);
  float y = (aPos[1] + ((float)aSize[1])/2 - ry)/((float)rh);
  
  float waspect = ((float)rw)/((float)rh);
  
  center(SbVec2f(x,y),waspect);
  
  // Arrange so that at least what had been selected 
  // will be fully visible. Except in case the
  // aspect ratio of the displaying window
  // and the one of the selected area match perfectly, 
  // more than what had been selected will be visible.

  float raspect = ((float)aSize[0])/((float)aSize[1]);
  float factor = 1;
  if(raspect>=waspect) {
    factor = ((float)aSize[0])/((float)rw);
  } else {
    factor = ((float)aSize[1])/((float)rh);
  }
  soCamera->scaleHeight(factor);
}

void SoRegion::center(const SbVec2f& aPos,float aAspect){
  // aPos is in NDC ([0,1][0,1]).
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;
  
  SbViewVolume vv;
  int mapping = soCamera->viewportMapping.getValue();
  if(mapping==SoCamera::ADJUST_CAMERA) { //3D (camera default).
    vv = soCamera->getViewVolume(aAspect);
    if (aAspect < 1.0f) vv.scale(1.0f / aAspect);
  } else { // SoCamera::LEAVE_ALONE //2D (plotting).
    vv = soCamera->getViewVolume(0.0f);
  }
  
  SbPlane panningplane = vv.getPlane(soCamera->focalDistance.getValue());
  SbLine line;

  SbVec2f center(0.5,0.5);
  vv.projectPointToLine(center,line);
  SbVec3f centerWC;
  panningplane.intersect(line,centerWC);
  vv.projectPointToLine(aPos,line);
  SbVec3f posWC;
  panningplane.intersect(line,posWC);
  soCamera->position = soCamera->position.getValue() + (posWC - centerWC);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoRegion::getTopSeparator() const {return 0;}
SoCamera* SoRegion::getCamera() const {return 0;}
void SoRegion::setCamera(SoCamera*){}
void SoRegion::clear(const SbString&){}
void SoRegion::resetCamera(){}
SbBool SoRegion::doIt(const SbDoIt&){return FALSE;}
void SoRegion::undo(){}
void SoRegion::resetUndo(){}
SbBool SoRegion::connectFrom(SoRegion*){return FALSE;}

SbBool SoRegion::setNode(const SbString& aWhat,const SbString& aSearch,const SbString& aValue) { 
  SoNode* node = findNode(aWhat,aSearch);
  if(!node) return FALSE;
  return node->set(aValue.getString());
}

void SoRegion::setPosition(float aX,float aY) {
  SoViewportRegion* vpr = getViewportRegion();
  vpr->setPositionPercent(aX,aY);
}

void SoRegion::setSize(float aW,float aH) {
  SoViewportRegion* vpr = getViewportRegion();
  vpr->setSizePercent(aW,aH);
}

void SoRegion::updateChildren(){} //should be pure virtual.

void SoRegion::refresh(){
  raiseUpdateFlag();
  updateChildren();  
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

RegionCamera::RegionCamera()
:fViewportMapping(0)
,fOrientation(0)
,fAspectRatio(0)
,fNearDistance(0)
,fFarDistance(0)
,fFocalDistance(0)
,fHeight(0)
{
  fPosition.setValue(0,0,0);
}

RegionCamera::RegionCamera(const SoCamera& aCamera)
:fHeight(0)
{
  fViewportMapping = aCamera.viewportMapping.getValue();
  fPosition = aCamera.position.getValue();
  fOrientation = aCamera.orientation.getValue();
  fAspectRatio = aCamera.aspectRatio.getValue();
  fNearDistance = aCamera.nearDistance.getValue();
  fFarDistance = aCamera.farDistance.getValue();
  fFocalDistance = aCamera.focalDistance.getValue();
  if(aCamera.isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    fHeight = ((SoOrthographicCamera*)&aCamera)->height.getValue();
  } else if(aCamera.isOfType(SoPerspectiveCamera::getClassTypeId())==TRUE) {
    fHeight = ((SoPerspectiveCamera*)&aCamera)->heightAngle.getValue();
  }
}

RegionCamera::RegionCamera(const RegionCamera& aCamera)
:fViewportMapping(aCamera.fViewportMapping)
,fPosition(aCamera.fPosition)
,fOrientation(aCamera.fOrientation)
,fAspectRatio(aCamera.fAspectRatio)
,fNearDistance(aCamera.fNearDistance)
,fFarDistance(aCamera.fFarDistance)
,fFocalDistance(aCamera.fFocalDistance)
,fHeight(aCamera.fHeight)
{}

RegionCamera::~RegionCamera(){}

RegionCamera& RegionCamera::operator=(const RegionCamera& aCamera){
  fViewportMapping = aCamera.fViewportMapping;
  fPosition = aCamera.fPosition;
  fOrientation = aCamera.fOrientation;
  fAspectRatio = aCamera.fAspectRatio;
  fNearDistance = aCamera.fNearDistance;
  fFarDistance = aCamera.fFarDistance;
  fFocalDistance = aCamera.fFocalDistance;
  fHeight = aCamera.fHeight;
  return *this;
}

void RegionCamera::restore(SoCamera& aCamera){
  aCamera.viewportMapping.setValue(fViewportMapping);
  aCamera.position.setValue(fPosition);
  aCamera.orientation.setValue(fOrientation);
  aCamera.aspectRatio.setValue(fAspectRatio);
  aCamera.nearDistance.setValue(fNearDistance);
  aCamera.farDistance.setValue(fFarDistance);
  aCamera.focalDistance.setValue(fFocalDistance);
  if(aCamera.isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    ((SoOrthographicCamera*)&aCamera)->height.setValue(fHeight);
  } else if(aCamera.isOfType(SoPerspectiveCamera::getClassTypeId())==TRUE) {
    ((SoPerspectiveCamera*)&aCamera)->heightAngle.setValue(fHeight);
  }
}

//FIXME : the below "PAW placement" is used in SoPage with 
// regionsResizePolicy set to PAW_GRID. 
//  We should arrange to avoid it here. We should be able
// to create, wihtin SoPage, a "SoPAW_Region" inheriting 
// SoRegion and handling such informations.
void SoRegion::setPAW_Placement(int aRows,int aCols,int aIndex) { 
  fPAW_Rows = aRows;
  fPAW_Cols = aCols;
  fPAW_Index = aIndex;
}

void SoRegion::getPAW_Placement(int& aRows,int& aCols,int& aIndex) const{ 
  aRows = fPAW_Rows;
  aCols = fPAW_Cols;
  aIndex = fPAW_Index;
}

void SoRegion::placeInPage(
 float //aPageWidth
,float //aPageHeight
,float //aWidth 
,float //aHeight
,float //aLeftMargin
,float //aRightMargin
,float //aTopMargin
,float //aBottomMargin
){}

SbBool SoRegion::readScene(
 const SbString& //aFile
,const SbString& //aFormat
,const SbString& //aPlacement
){
  SoDebugError::postInfo("SoRegion::readScene","no implementation.\n");
  return FALSE;
}

SoSeparator* SoRegion::getAlternateRep() {
  SoSeparator* soSep = new SoSeparator;
  soSep->addChild(viewportRegion.getValue());
  soSep->addChild(getTopSeparator());
  return soSep;
}

void SoRegion::raiseUpdateFlag() {fUpdate = TRUE;}
SoNode* SoRegion::getNodeOfInterest(const SbString&) const {return getTopSeparator();}

//DEPRECATED.
SoNode* SoRegion::getWriteNode(const SbString& aWhat) const {return getNodeOfInterest(aWhat);}

