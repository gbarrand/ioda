// this :
#include <HEPVis/nodekits/SoCanvas.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoLists.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/draggers/SoDragger.h>

// HEPVis :
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/nodekits/SoRegion.h>
#include <HEPVis/misc/SoTools.h>
#include <HEPVis/actions/SoCollectAction.h>

#define NotFound (-1)

SO_KIT_SOURCE(SoCanvas) 
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoCanvas,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoCanvas::SoCanvas(
)
:fSetRegionCallback(0)
,fCollectCallback(0)
,fHandlePickedPoint(TRUE)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoCanvas);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(verbose,(FALSE));

  SoEventCallback* eventCallback = new SoEventCallback;
  eventCallback->addEventCallback(SoEvent::getClassTypeId(),eventCB,this);
  eventCallback->addEventCallback(SoKeyboardEvent::getClassTypeId(),keyboardEventCB,this);
  if(setPart("callbackList[0]",eventCallback)==FALSE) eventCallback->unref(); 
}
//////////////////////////////////////////////////////////////////////////////
SoCanvas::~SoCanvas(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fSetRegionCallback) fSetRegionCallback->setCamera(0);
  delete fCollectCallback;
  delete fSetRegionCallback;
}
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoCanvas::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setHandlePickedPoint(
 SbBool aHandlePickedPoint
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fHandlePickedPoint = aHandlePickedPoint;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::handlePickedPoint(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fHandlePickedPoint;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setCollectCallback(
 SbCollectCallback* aCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete fCollectCallback;
  fCollectCallback = aCallback;
}
//////////////////////////////////////////////////////////////////////////////
SbCollectCallback* SoCanvas::getCollectCallback(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fCollectCallback;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::keyboardEventCB(
 void* aThis
,SoEventCallback* aEventCB
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aEventCB->isHandled()) return;
  SoCanvas* This = (SoCanvas*)aThis;
  const SoKeyboardEvent* event = (SoKeyboardEvent*)aEventCB->getEvent();
  if(SO_KEY_PRESS_EVENT(event,RIGHT_ARROW)) {
    This->highlightNextRegion();
    aEventCB->setHandled();
  } else if(SO_KEY_PRESS_EVENT(event,LEFT_ARROW)) {
    This->highlightPreviousRegion();
    aEventCB->setHandled();
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::eventCB(
 void* aThis
,SoEventCallback* aEventCB
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aEventCB->isHandled()) return;
  SoCanvas* This = (SoCanvas*)aThis;
  const SoEvent* event = aEventCB->getEvent();
  if(This->dispatchEvent(event)==TRUE) {
    aEventCB->setHandled();
    return;
  }
  if(event->isOfType(SoMouseButtonEvent::getClassTypeId())) {

  // Treat simple button1 pick without modifiers. SoDetectorTreeKit picking
  // needs some modifier.
  // WARNING : for Coin, BUTTON2 is right button, BUTTON3 is middle button.
  if(SO_MOUSE_PRESS_EVENT(event,BUTTON1)) {
    if(!event->wasCtrlDown() && !event->wasShiftDown()) {
  
      const SbVec2s& pos = event->getPosition();

      if(This->verbose.getValue()==TRUE) { 
        SoDebugError::postInfo("SoCanvas::mouseEventCB",
          "BUTTON1 pressed without Ctrl or Shift down. Position %d %d",
          pos[0],pos[1]);
      }

      SoRegion* soRegion = This->getPickedRegion(pos[0],pos[1]);
      if(!soRegion) return;

      This->highlightRegion(soRegion);

      if(This->handlePickedPoint()==FALSE) {
        This->setHandlePickedPoint(TRUE);
        return;
      }

      const SoPickedPoint* pickedPoint = aEventCB->getPickedPoint();
      if(pickedPoint==NULL) {
        SoTools::resetHighlight(soRegion);
        SoNodeList nodeList;
        if(This->fCollectCallback) This->fCollectCallback->execute(*This,nodeList);
        aEventCB->setHandled();
      } else if(SoFullPath* path = (SoFullPath*)pickedPoint->getPath()) {
        if(This->getPathRegion(*path)!=soRegion) {
          SoTools::resetHighlight(soRegion);
          SoNodeList nodeList;
          if(This->fCollectCallback) This->fCollectCallback->execute(*This,nodeList);
          aEventCB->setHandled();
        }

        SoNode* node = path->getTail();

        if(This->verbose.getValue()==TRUE) { 
          if(node) {
            SoDebugError::postInfo("SoCanvas::mouseEventCB",
                         "picked node %lu of SoType \"%s\" with name \"%s\".",
                                 node,
                                 node->getTypeId().getName().getString(),
                                 node->getName().getString());
          }
        }
    
       {for (int i=0;i<path->getLength();i++) {
          SoNode* n = path->getNodeFromTail(i);
          //If in a dragger, the dragging shape may be picked !
          if(n->isOfType(SoDragger::getClassTypeId()))  return;
        }}

        SoNodeList nodeList;
        if(node) nodeList.append(node);
        SoTools::highlight(*path);
        if(This->fCollectCallback) This->fCollectCallback->execute(*This,nodeList);
    
        aEventCB->setHandled();
      }

    }
  } else if(SO_MOUSE_PRESS_EVENT(event,BUTTON2)) {
    //RightButton = Contextual popup.
  
    if(This->verbose.getValue()==TRUE) { 
      SoDebugError::postInfo("SoCanvas::mouseEventCB","BUTTON2 pressed.");
    }

    const SbVec2s& pos = event->getPosition();
    SoRegion* soRegion = This->getPickedRegion(pos[0],pos[1]);
    if(!soRegion) return;

    This->highlightRegion(soRegion);

    const SoPickedPoint* pickedPoint = aEventCB->getPickedPoint();
    if(pickedPoint) {
      if(SoFullPath* path = (SoFullPath*)pickedPoint->getPath()) {
        if(This->getPathRegion(*path)!=soRegion) {
          SoTools::resetHighlight(soRegion);
          SoNodeList nodeList;
          if(This->fCollectCallback) This->fCollectCallback->execute(*This,nodeList);
          aEventCB->setHandled();
        }

        SoNode* node = path->getTail();

        if(This->verbose.getValue()==TRUE) { 
          if(node) {
            SoDebugError::postInfo("SoCanvas::mouseEventCB",
                         "picked node %lu of SoType \"%s\" with name \"%s\".",
                               node,
                               node->getTypeId().getName().getString(),
                               node->getName().getString());
          }
        }
    
       {for (int i=0;i<path->getLength();i++) {
          SoNode* n = path->getNodeFromTail(i);
          //If in a dragger, the dragging shape may be picked !
          if(n->isOfType(SoDragger::getClassTypeId()))  return;
        }}

        SoTools::resetHighlight(soRegion);
        SoNodeList nodeList;
        if(node) nodeList.append(node);
        SoTools::highlight(*path);
        if(This->fCollectCallback) 
          This->fCollectCallback->execute(*This,nodeList);
        aEventCB->setHandled();
      }
    }

  }

  }

}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::dispatchEvent(
 const SoEvent* aEvent
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction action;
  action.setFind(SoSearchAction::TYPE);
  action.setType(SoViewportRegion::getClassTypeId());
  action.setInterest(SoSearchAction::ALL);
  action.apply(topSeparator.getValue());
  SoBaseKit::setSearchingChildren(oldsearch);
  
  SoPathList& pathList = action.getPaths();
  int pathn = pathList.getLength();
  // Reverse order since last drawn is the one in front.
  for(int index=pathn-1;index>=0;index--) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoViewportRegion* viewportRegion = (SoViewportRegion*)path->getTail();
    if(viewportRegion->dispatchEvent(aEvent)==TRUE) return TRUE;
  }

  return FALSE;
}

/*
void SoCanvas::enforceWinSize(const SbVec2s& aSize){
  //used with SoOffscreenRenderer.

  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  SoSearchAction action;
  action.setFind(SoSearchAction::TYPE);
  action.setType(SoViewportRegion::getClassTypeId());
  action.setInterest(SoSearchAction::ALL);
  action.apply(topSeparator.getValue());
  SoBaseKit::setSearchingChildren(oldsearch);
  
  SoPathList& pathList = action.getPaths();
  int pathn = pathList.getLength();

  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoViewportRegion* viewportRegion = (SoViewportRegion*)path->getTail();
    viewportRegion->enforceWinSize(aSize);
  }
}
*/

//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setSetRegionCallback(
 SbSetRegionCallback* aCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fSetRegionCallback = aCallback;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::disableRegionCamera(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fSetRegionCallback) fSetRegionCallback->setCamera(0);
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::createRegion(
 const SbString& aClass
,float aX
,float aY
,float aWidth
,float aHeight                           
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* sep = (SoSeparator*)topSeparator.getValue();
  SoType type = SoType::fromName(aClass);
  if(type.isBad()==TRUE) {
    SoDebugError::postInfo
      ("SoCanvas::createRegion","%s is a bad type.",aClass.getString());
    return 0;
  }
  if(type.isDerivedFrom(SoRegion::getClassTypeId())==FALSE) {
    SoDebugError::postInfo
      ("SoCanvas::createRegion","%s is not an SoRegion.",aClass.getString());
    return 0;
  }
  SoRegion* soRegion = (SoRegion*)type.createInstance();
  if(!soRegion) {
    SoDebugError::postInfo
      ("SoCanvas::createRegion","can't create instance of %s.",
      aClass.getString());
    return 0;
  }
  soRegion->verbose.setValue(verbose.getValue());
  sep->addChild(soRegion);
  SoViewportRegion* viewportRegion  = 
    (SoViewportRegion*)soRegion->getViewportRegion();
  viewportRegion->setPositionPercent(aX,aY);
  viewportRegion->setSizePercent(aWidth,aHeight);
  return soRegion;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::deleteRegion(
 SoRegion* aRegion
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(aRegion==soRegion) {
      if(path->getLength()>=2) {
        SoNode* parent = path->getNodeFromTail(1);
        if(parent->isOfType(SoGroup::getClassTypeId())) {
          ((SoGroup*)parent)->removeChild(soRegion);
        }
      }
      return;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::clearRegions(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // A region.clear() like a SoPlotterRegion.clear() may ask for 
  // the deletion of SoRegion already in the pathList (for exa 
  // the SoLegendRegions in the legendRegions kit list). 
  // Through an auditor mechanism the pathList is informed and 
  // then the getTail may be not a SoRegion anymore. Then we test 
  // the class of the getTail to ensure to have an SoRegion.

  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoNode* soNode = path->getTail();
    if(soNode->isOfType(SoRegion::getClassTypeId())==TRUE) {
      SoRegion* soRegion = (SoRegion*)soNode;
      soRegion->clear();
    }
  }
}

void SoCanvas::raiseUpdateFlagOfRegions(){
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    soRegion->raiseUpdateFlag();
  }
}

void SoCanvas::refreshRegions(){
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    soRegion->refresh();
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::deleteRegions(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fSetRegionCallback) fSetRegionCallback->setCamera(0);
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(path->getLength()>=2) {
      SoNode* parent = path->getNodeFromTail(1);
      if(parent->isOfType(SoGroup::getClassTypeId())) {
        ((SoGroup*)parent)->removeChild(soRegion);
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getPickedRegion(
 int aX
,int aY
) const
//////////////////////////////////////////////////////////////////////////////
// aX, aY should be in GL coordinates.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  // Reverse order since last drawn is the one in front.
  for(int index=pathn-1;index>=0;index--) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion->visible.getValue()==FALSE) continue;
    SoViewportRegion* soViewportRegion = soRegion->getViewportRegion();
    if(soViewportRegion->isInside(aX,aY)==TRUE) return soRegion;
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setRegionsCanvasVerbosity(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    soRegion->verbose.setValue(verbose.getValue());
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setRootRegionsHighlightable(
 SbBool aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) { //root regions.
      SoRegion* soRegion = (SoRegion*)path->getTail();
      soRegion->verbose.setValue(verbose.getValue());
      SoViewportRegion* soViewportRegion = soRegion->getViewportRegion();
      soViewportRegion->showHighlight.setValue(aValue);
    //soViewportRegion->sensitiveBorder.setValue(FALSE);
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
int SoCanvas::getNumberOfRegions(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  return pathList.getLength();
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getRegion(
 int aIndex
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  if((aIndex<0)||(aIndex>=pathn)) return 0;
  SoFullPath* path = (SoFullPath*)pathList[aIndex];
  return (SoRegion*)path->getTail();
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getNextRegion(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion==aRegion) {
      int inext = ((index+1)<pathn?index+1:0); 
      if(inext!=index) {
        SoFullPath* nextPath = (SoFullPath*)pathList[inext];
        return (SoRegion*)nextPath->getTail();
      }
      return 0;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getPreviousRegion(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion==aRegion) {
      int iprev = ((index-1)>=0?index-1:pathn-1); 
      if(iprev!=index) {
        SoFullPath* prevPath = (SoFullPath*)pathList[iprev];
        return (SoRegion*)prevPath->getTail();
      }
      return 0;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
int SoCanvas::getRegionIndex(
  SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion==aRegion) return index;
  }
  return NotFound;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::setRegionOnTop(
  SoRegion* aRegion
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion==aRegion) {
      if(path->getLength()>=2) {
        SoNode* parent = path->getNodeFromTail(1);
        if(parent->isOfType(SoGroup::getClassTypeId())) {
          soRegion->ref();
          ((SoGroup*)parent)->removeChild(soRegion);
          int number = ((SoGroup*)parent)->getNumChildren();
          ((SoGroup*)parent)->insertChild(soRegion,number);
          soRegion->unref();
        }
      }
      return;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getPathRegion(
 const SoFullPath& aPath
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int l = aPath.getLength();
  for(int index=1;index<l;index++) {
    SoNode* n = aPath.getNodeFromTail(index);
    if(n->isOfType(SoRegion::getClassTypeId())) {
      return (SoRegion*)n;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::collect(
 int aX
,int aY
,unsigned int aW
,unsigned int aH
,const SbViewportRegion& aVP
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* node = getHighlightedRegion();
  if(!node) return;
  SoCollectAction action(aVP);
  action.setSearchArea(aX,aY,aW,aH);
  action.apply(node);
  SoNodeList nodeList;
  SoPathList& pathList = action.paths();
  int pathn = pathList.getLength();
  if(!pathn) {
    SoTools::resetHighlight(node);
  } else {
    for(int index=0;index<pathn;index++) {
      SoFullPath* path = (SoFullPath*)pathList[index];
      SoNode* node = path->getTail();
      if(node) {        
        nodeList.append(node);
        SoTools::highlight(*path);
        //printf("(path %p, len %d)  ", path, path->getLength());
        //for (int i=0; i < path->getLength(); i++) {
        //SoNode* n = path->getNodeFromTail(i);
        //printf("%p (%s), ",n, n->getTypeId().getName().getString());
        //}
        //printf("\n");
      }
    }
  }
  if(fCollectCallback) fCollectCallback->execute(*this,nodeList);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////// Actions over highlighted region /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getHighlightedRegion(
) const
//////////////////////////////////////////////////////////////////////////////
// Get the first highlighted SoRegion.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  // Reverse order since there is more
  // chance that the one in front is the highlighted one.
  for(int index=pathn-1;index>=0;index--) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion->getViewportRegion()->highlighted.getValue()==TRUE) {
      return soRegion;
    }
  }

  return 0;
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::highlightRegion(
 SoRegion* aRegion
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* soRegion = getHighlightedRegion();
  SoCamera* oldCamera = 0;
  if(soRegion) {
    if(aRegion!=soRegion) {
      SoViewportRegion* viewportRegion = soRegion->getViewportRegion();
      viewportRegion->highlighted.setValue(FALSE);
      oldCamera = soRegion->getCamera();
      if(oldCamera) oldCamera->ref();
    }
  }
  SoViewportRegion* viewportRegion = aRegion->getViewportRegion();
  viewportRegion->highlighted.setValue(TRUE);
  if(fSetRegionCallback) {
    // The below may remove existing viewer camera from 
    // its parent, then letting the soRegion without camera !
    fSetRegionCallback->setCamera(aRegion->getCamera());
  }
  if(soRegion && oldCamera) {
    soRegion->setCamera(oldCamera);
    oldCamera->unref();
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::deleteHighlightedRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* highlightedRegion = getHighlightedRegion();
  if(!highlightedRegion) return;
  SoRegion* prevRegion = getPreviousRegion(highlightedRegion);
  if(prevRegion) {
    // WARNING : prevRegion could be a child of the highlightedRegion !
    //           ( for example a SoLegendRegion of a SoPlotterRegion)
    //           then we do a ref() to be sure to have still
    //           a valid object in the below highlightRegion(prevRegion);
    prevRegion->ref();
    //Try with the ref() logic. The below isChildOf is more heavy.
    //if(SoTools::isChildOf(highlightedRegion,prevRegion)) prevRegion = 0;
  }
  if(fSetRegionCallback) fSetRegionCallback->setCamera(0);
  deleteRegion(highlightedRegion);
  if(prevRegion) {
    highlightRegion(prevRegion);
    prevRegion->unref();
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::highlightNextRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* highlightedRegion = getHighlightedRegion();
  if(!highlightedRegion) {
    SoRegion* soRegion = getRegion(0);
    if(soRegion) highlightRegion(soRegion);
  } else {
    SoRegion* soRegion = getNextRegion(highlightedRegion);
    if(soRegion) highlightRegion(soRegion);   
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCanvas::highlightPreviousRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* highlightedRegion = getHighlightedRegion();
  if(!highlightedRegion) {
    SoRegion* soRegion = getRegion(0);
    if(soRegion) highlightRegion(soRegion);
  } else {
    SoRegion* soRegion = getPreviousRegion(highlightedRegion);
    if(soRegion) highlightRegion(soRegion);   
  }
}
//////////////////////////////////////////////////////////////////////////////
//////// Root regions ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int SoCanvas::getNumberOfRootRegions(
) const
//////////////////////////////////////////////////////////////////////////////
// A root region is a region without a parent region.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  int iroot = 0;
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) iroot++;
  }
  return iroot;
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getRootRegion(
 int aIndex
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  int iroot = 0;
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) {
      if(iroot==aIndex) return (SoRegion*)path->getTail();
      iroot++;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::isRootRegion(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    SoRegion* soRegion = (SoRegion*)path->getTail();
    if(soRegion==aRegion) {
      if(!getPathRegion(*path)) return TRUE;
      return FALSE;
    }
  }
  return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
int SoCanvas::getRootRegionIndex(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  int iroot = 0;
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) {
      SoRegion* soRegion = (SoRegion*)path->getTail();
      if(soRegion==aRegion) return iroot;
      iroot++;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getNextRootRegion(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNodeList nodeList;
 {SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) {
      SoRegion* soRegion = (SoRegion*)path->getTail();
      nodeList.append(soRegion);
    }
  }}
  int number = nodeList.getLength();
  for(int index=0;index<number;index++) {
    if(aRegion==nodeList[index]) {
      int inext = ((index+1)<number?index+1:0); 
      if(inext!=index) return (SoRegion*)nodeList[inext];
      return 0;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::getPreviousRootRegion(
 SoRegion* aRegion
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNodeList nodeList;
 {SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) {
      SoRegion* soRegion = (SoRegion*)path->getTail();
      nodeList.append(soRegion);
    }
  }}
  int number = nodeList.getLength();
  for(int index=0;index<number;index++) {
    if(aRegion==nodeList[index]) {
      int iprev = ((index-1)>=0?index-1:number-1); 
      if(iprev!=index) return (SoRegion*)nodeList[iprev];
      return 0;
    }
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////
//////// Current regions /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// The "current region" is an highlighted region.
// It is not necessarily (since OSC-16.2) a root region.
// (For example to be able to write a non root region from OnX/Scene.onx).
//////////////////////////////////////////////////////////////////////////////
SoRegion* SoCanvas::currentRegion(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* highlighted = getHighlightedRegion();
  if(!highlighted) return 0;
  //if(isRootRegion(highlighted)==FALSE) return 0;
  return highlighted;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::setCurrentRegion(
 int aIndex
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* soRegion = getRootRegion(aIndex);
  if(!soRegion) return FALSE;
  highlightRegion(soRegion);
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::setCurrentRegion(
 SoRegion* aRegion
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(isRootRegion(aRegion)==FALSE) return FALSE;
  highlightRegion(aRegion);
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SoCanvas::connectCurrentRegion(
 int aIndex
)
//////////////////////////////////////////////////////////////////////////////
// Connect the current region to the "aIndex" region.
// The current region will then display the scene graph of the "aIndex" region.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoRegion* region = currentRegion();
  if(!region) return FALSE;
  SoRegion* fromRegion = getRootRegion(aIndex);
  if(!fromRegion) return FALSE;
  return region->connectFrom(fromRegion);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoPathList& SoCanvas::findRegions(
 SoSearchAction& aAction
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbBool oldsearch = SoBaseKit::isSearchingChildren();
  SoBaseKit::setSearchingChildren(TRUE);
  aAction.setFind(SoSearchAction::TYPE);
  aAction.setType(SoRegion::getClassTypeId());
  aAction.setInterest(SoSearchAction::ALL);
  aAction.apply(topSeparator.getValue());
  SoBaseKit::setSearchingChildren(oldsearch);
  return aAction.getPaths();
}
